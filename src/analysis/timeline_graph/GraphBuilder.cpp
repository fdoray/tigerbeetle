/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "analysis/timeline_graph/GraphBuilder.hpp"

#include <iostream>

#include "base/Constants.hpp"
#include "value/MakeValue.hpp"
#include "value/Utils.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

GraphBuilder::GraphBuilder()
    : _ts(0) {
}

GraphBuilder::~GraphBuilder() {
}

void GraphBuilder::SetQuarks(quark::StringQuarkDatabase* quarks)
{
    Q_TID = quarks->StrQuark(kTid);
    Q_PPID = quarks->StrQuark(kStatePpid);
    Q_DURATION = quarks->StrQuark(kDuration);
    Q_NODE_TYPE = quarks->StrQuark(kNodeType);
    Q_START_TIME = quarks->StrQuark(kStartTime);
    Q_STACK_DEPTH = quarks->StrQuark(kStackDepth);
}

void GraphBuilder::SetTimestamp(timestamp_t ts) {
    _ts = ts;
}

bool GraphBuilder::AddGraph(ThreadId thread_id, const std::string& description) {
    // A graph already exists for this thread.
    if (_last_node_for_thread_id.find(thread_id) != _last_node_for_thread_id.end()) {
        return false;
    }

    // Create the new graph with an initial node.
    auto graph_and_properties = GraphAndProperties::UP {new GraphAndProperties};
    auto& node = graph_and_properties->graph.CreateNode();
    auto graph_index = _graphs.size();
    graph_and_properties->description = description;
    _graphs.push_back(std::move(graph_and_properties));

    // Keep track of the last node for the root thread.
    _last_node_for_thread_id[thread_id] = NodeKey(graph_index, node.id());
    SetProperty(thread_id, Q_TID, value::MakeValue(thread_id));
    StartTimer(thread_id, Q_DURATION);

    return true;
}

bool GraphBuilder::AddChildTask(ThreadId parent_thread_id, TaskId child_task_id) {
    size_t graph_index = 0;
    Node* parent_node = nullptr;
    if (!GetLastNodeForThread(parent_thread_id, &graph_index, &parent_node))
        return false;

    // Create an "arrow" node for the new task.
    auto& child_node = _graphs[graph_index]->graph.CreateNode();
    parent_node->set_vertical_child(child_node.id());
    _pending_tasks[child_task_id] = NodeKey(graph_index, child_node.id());

    _graphs[graph_index]->properties.SetProperty(child_node.id(), Q_NODE_TYPE, value::MakeValue(kArrow));
    _graphs[graph_index]->properties.SetProperty(child_node.id(), Q_PPID, value::MakeValue(parent_thread_id));
    _graphs[graph_index]->properties.SetProperty(child_node.id(), Q_TID, value::MakeValue(-1));
    _graphs[graph_index]->properties.SetProperty(child_node.id(), Q_START_TIME, value::MakeValue(_ts));
    _graphs[graph_index]->properties.SetProperty(child_node.id(), Q_DURATION, value::MakeValue(static_cast<uint64_t>(0ull)));

    // Create a step in the parent task.
    AddStepForThreadId(parent_thread_id);

    return true;
}

bool GraphBuilder::ScheduleTask(TaskId task_id, ThreadId thread_id) {
    auto look = _pending_tasks.find(task_id);
    if (look == _pending_tasks.end())
        return false;

    // Pop everything on this thread. //////////////// TODO
    while (PopStack(thread_id))
        continue;

    const NodeKey& node_key = look->second;
    Node& arrow_node = _graphs[node_key.graph_index]->graph.GetNode(node_key.node_id);

    // Schedule the task.
    _last_node_for_thread_id[thread_id] = node_key;
    SetProperty(thread_id, Q_TID, value::MakeValue(thread_id));

    // Set the duration of the arrow.
    uint64_t arrowStartTime = GetProperty(thread_id, Q_START_TIME)->AsULong();
    uint64_t arrowDuration = _ts - arrowStartTime;
    SetProperty(thread_id, Q_DURATION, value::MakeValue(arrowDuration));
    SetProperty(thread_id, Q_TID, value::MakeValue(thread_id));

    // Create the first node of the task.
    Node& new_node = _graphs[node_key.graph_index]->graph.CreateNode();
    arrow_node.set_horizontal_child(new_node.id());
    _last_node_for_thread_id[thread_id] = NodeKey(node_key.graph_index, new_node.id());

    StartTimer(thread_id, Q_DURATION);
    SetProperty(thread_id, Q_TID, value::MakeValue(thread_id));

    // Remove from the map of pending tasks.
    _pending_tasks.erase(look);

    return true;
}

bool GraphBuilder::AddStepForThreadId(ThreadId thread_id) {
    size_t graph_index = 0;
    Node* previous_node = nullptr;
    if (!GetLastNodeForThread(thread_id, &graph_index, &previous_node))
        return false;

    auto previousNodeType = GetProperty(thread_id, Q_NODE_TYPE);

    // Read and reset the timers.
    ReadAndResetTimers(thread_id);

    // Create the next node for the task.
    auto& next_node = _graphs[graph_index]->graph.CreateNode();
    previous_node->set_horizontal_child(next_node.id());
    _last_node_for_thread_id[thread_id] = NodeKey(graph_index, next_node.id());

    // Set tid and node type for the new node.
    SetProperty(thread_id, Q_TID, value::MakeValue(static_cast<uint32_t>(thread_id)));
    if (previousNodeType != nullptr)
        SetProperty(thread_id, Q_NODE_TYPE, previousNodeType->Copy());

    return true;
}

bool GraphBuilder::EndTaskOnThread(ThreadId thread_id) {
    // Read and reset the timers.
    ReadAndResetTimers(thread_id);
    _timers.erase(thread_id);
    return (_last_node_for_thread_id.erase(thread_id) != 0);
}

bool GraphBuilder::PushStack(ThreadId thread_id) {
    if (!AddStepForThreadId(thread_id))
        return false;

    auto previousNodeTypeValue = GetProperty(thread_id, Q_NODE_TYPE);
    std::string previousNodeType;
    if (previousNodeTypeValue != nullptr)
        previousNodeType = previousNodeTypeValue->AsString();

    int32_t previousStackDepth = static_cast<int32_t>(_stacks[thread_id].size());
    _stacks[thread_id].push(previousNodeType);

    SetProperty(thread_id, Q_STACK_DEPTH, value::MakeValue(previousStackDepth + 1));

    return true;
}

bool GraphBuilder::PopStack(ThreadId thread_id) {
    auto stack_it = _stacks.find(thread_id);
    if (stack_it == _stacks.end())
    {
        return EndTaskOnThread(thread_id);
    }
    int32_t previousStackDepth = static_cast<int32_t>(stack_it->second.size());
    assert(previousStackDepth > 0);

    if (!AddStepForThreadId(thread_id))
        return false;

    SetProperty(thread_id, Q_STACK_DEPTH, value::MakeValue(previousStackDepth - 1));
    SetProperty(thread_id, Q_NODE_TYPE, value::MakeValue(stack_it->second.top()));
    stack_it->second.pop();

    if (stack_it->second.empty())
        _stacks.erase(stack_it);

    return true;
}

bool GraphBuilder::StartTimer(ThreadId thread_id, quark::Quark timer_name) {
    if (_last_node_for_thread_id.find(thread_id) ==
            _last_node_for_thread_id.end()) {
        return false;
    }

    _timers[thread_id].StartTimer(_ts, timer_name);

    return true;
}

bool GraphBuilder::StopTimer(ThreadId thread_id, quark::Quark timer_name) {
    if (_last_node_for_thread_id.find(thread_id) ==
            _last_node_for_thread_id.end()) {
        return false;
    }

    uint64_t elapsed_time = 0;
    if (!_timers[thread_id].ReadAndStopTimer(_ts, timer_name, &elapsed_time))
        return false;

    size_t graph_index = 0;
    Node* node = nullptr;
    if (!GetLastNodeForThread(thread_id, &graph_index, &node))
        return false;
    if (elapsed_time != 0)
    {
        _graphs[graph_index]->properties.IncrementProperty(node->id(), timer_name, elapsed_time);
    }

    return true;
}

uint64_t GraphBuilder::ReadTimer(ThreadId thread_id, quark::Quark timer_name) {
    if (_last_node_for_thread_id.find(thread_id) ==
            _last_node_for_thread_id.end()) {
        return 0;
    }

    uint64_t elapsed_time = 0;
    if (!_timers[thread_id].ReadTimer(_ts, timer_name, &elapsed_time))
        return 0;

    return elapsed_time;
}

bool GraphBuilder::IncrementProperty(ThreadId thread_id, quark::Quark property, uint64_t increment)
{
    size_t graph_index = 0;
    Node* node = nullptr;
    if (!GetLastNodeForThread(thread_id, &graph_index, &node))
        return false;

    _graphs[graph_index]->properties.IncrementProperty(node->id(), property, increment);

    return true;
}

bool GraphBuilder::SetProperty(ThreadId thread_id, quark::Quark property, value::Value::UP value)
{
    size_t graph_index = 0;
    Node* node = nullptr;
    if (!GetLastNodeForThread(thread_id, &graph_index, &node))
        return false;

    _graphs[graph_index]->properties.SetProperty(node->id(), property, std::move(value));

    return true;
}

value::Value* GraphBuilder::GetProperty(ThreadId thread_id, quark::Quark property)
{
    size_t graph_index = 0;
    Node* node = nullptr;
    if (!GetLastNodeForThread(thread_id, &graph_index, &node))
        return nullptr;

    return _graphs[graph_index]->properties.GetProperty(node->id(), property);
}

bool GraphBuilder::HasNodeForThread(ThreadId thread_id) const
{
    return _last_node_for_thread_id.find(thread_id) != _last_node_for_thread_id.end();
}

void GraphBuilder::Terminate()
{
    for (const auto& timer : _timers)
        ReadAndResetTimers(timer.first);
}

bool GraphBuilder::GetLastNodeForThread(ThreadId thread_id,
                                        size_t* graph_index,
                                        Node** node) {
    auto look = _last_node_for_thread_id.find(thread_id);
    if (look == _last_node_for_thread_id.end())
        return false;

    size_t graph_index_local = look->second.graph_index;
    if (graph_index != nullptr)
        *graph_index = graph_index_local;

    assert(_graphs.size() > graph_index_local);
    auto node_id = look->second.node_id;

    if (node != nullptr)
    {
        auto& graph = _graphs.at(graph_index_local);
        *node = &graph->graph.GetNode(node_id);
    }

    return true;
}

bool GraphBuilder::ReadAndResetTimers(ThreadId thread_id) {
    using namespace std::placeholders; 

    size_t graph_index = 0;
    Node* node = nullptr;
    if (!GetLastNodeForThread(thread_id, &graph_index, &node))
        return false;

    auto look = _timers.find(thread_id);
    if (look == _timers.end())
        return true;

    look->second.ReadAndResetTimers(
        _ts,
        std::bind(&TimelineGraphProperties::IncrementProperty,
                  &_graphs[graph_index]->properties,
                  node->id(),
                  _1,
                  _2));

    return true;
}

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee
