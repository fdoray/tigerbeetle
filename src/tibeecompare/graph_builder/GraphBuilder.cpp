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
#include "GraphBuilder.hpp"

namespace tibee
{

namespace {

const char kDurationTimerName[] = "duration";

}  // namespace

GraphBuilder::GraphBuilder()
    : _ts(0) {
    Reset();
}

GraphBuilder::~GraphBuilder() {
}

void GraphBuilder::Reset() {
    _last_node_for_task_id.clear();
    _graphs = GraphsUP {new Graphs};
    _ts = 0;
    _timers.clear();
}

std::unique_ptr<std::vector<GraphAndProperties::UP>>
    GraphBuilder::TakeGraphs() {
  return std::move(_graphs);
}

void GraphBuilder::SetTimestamp(common::timestamp_t ts) {
    _ts = ts;
}

bool GraphBuilder::AddGraph(TaskId task_id) {
    // A graph already exists for this task.
    if (_last_node_for_task_id.find(task_id) !=
            _last_node_for_task_id.end()) {
        return false;
    }

    // Create the new graph with an initial node.
    auto graph_and_properties = GraphAndProperties::UP {new GraphAndProperties};
    auto& node = graph_and_properties->graph.CreateNode();
    auto graph_index = _graphs->size();
    _graphs->push_back(std::move(graph_and_properties));

    // Keep track of the last node for the root task.
    _last_node_for_task_id[task_id] = NodeKey(graph_index, node.id());

    // Start a timer for the duration of the node.
    StartTimer(task_id, kDurationTimerName);

    return true;
}

bool GraphBuilder::AddTask(TaskId parent_task_id, TaskId child_task_id) {
    size_t graph_index = 0;
    GraphAndProperties* graph = nullptr;
    timeline_graph::TimelineNode* parent_node = nullptr;
    if (!GetLastNodeForTask(parent_task_id, &graph_index, &graph, &parent_node))
        return false;

    // Create a node for the new task.
    auto& child_node = graph->graph.CreateNode();
    parent_node->set_vertical_child(child_node.id());
    _last_node_for_task_id[child_task_id] =
        NodeKey(graph_index, child_node.id());

    // Start a timer for the duration of the child node.
    StartTimer(child_task_id, kDurationTimerName);

    // Create a step in the parent task.
    AddTaskStep(parent_task_id);

    return true;
}

bool GraphBuilder::AddTaskStep(TaskId task_id) {
    size_t graph_index = 0;
    GraphAndProperties* graph = nullptr;
    timeline_graph::TimelineNode* previous_node = nullptr;
    if (!GetLastNodeForTask(task_id, &graph_index, &graph, &previous_node))
        return false;

    // Read and reset the timers.
    ReadAndResetTimers(task_id);

    // Create the next node for the task.
    auto& next_node = graph->graph.CreateNode();
    previous_node->set_horizontal_child(next_node.id());
    _last_node_for_task_id[task_id] =
        NodeKey(graph_index, next_node.id());

    return true;
}

bool GraphBuilder::EndTask(TaskId task_id) {
    // Read and reset the timers.
    ReadAndResetTimers(task_id);
    _timers.erase(task_id);

    _last_node_for_task_id.erase(task_id);

    return true;
}

bool GraphBuilder::StartTimer(TaskId task_id, const std::string& timer_name) {
    if (_last_node_for_task_id.find(task_id) ==
            _last_node_for_task_id.end()) {
        return false;
    }

    _timers[task_id].StartTimer(_ts, timer_name);

    return true;
}

bool GraphBuilder::StopTimer(TaskId task_id, const std::string& timer_name) {
    if (_last_node_for_task_id.find(task_id) ==
            _last_node_for_task_id.end()) {
        return false;
    }

    uint64_t elapsed_time = 0;
    if (!_timers[task_id].ReadAndStopTimer(_ts, timer_name, &elapsed_time))
        return false;

    GraphAndProperties* graph = nullptr;
    timeline_graph::TimelineNode* node = nullptr;
    if (!GetLastNodeForTask(task_id, nullptr, &graph, &node))
        return false;
    graph->properties.IncrementProperty(node->id(), timer_name, elapsed_time);

    return true;
}

bool GraphBuilder::GetLastNodeForTask(TaskId task_id,
                                      size_t* graph_index,
                                      GraphAndProperties** graph,
                                      timeline_graph::TimelineNode** node) {
    auto look = _last_node_for_task_id.find(task_id);
    if (look == _last_node_for_task_id.end())
        return false;

    size_t graph_index_local = look->second.graph_index;
    if (graph_index != nullptr)
        *graph_index = graph_index_local;

    assert(_graphs->size() > graph_index_local);
    auto node_id = look->second.node_id;

    auto& graph_local =  _graphs->at(graph_index_local);

    if (graph != nullptr)
        *graph = graph_local.get();

    if (node != nullptr)
        *node = &(*graph)->graph.GetNode(node_id);

    return true;
}

bool GraphBuilder::ReadAndResetTimers(TaskId task_id) {
    using namespace std::placeholders; 

    GraphAndProperties* graph = nullptr;
    timeline_graph::TimelineNode* node = nullptr;
    if (!GetLastNodeForTask(task_id, nullptr, &graph, &node))
        return false;

    auto look = _timers.find(task_id);
    if (look == _timers.end())
        return true;

    look->second.ReadAndResetTimers(
        _ts,
        std::bind(&GraphProperties::IncrementProperty,
                  &graph->properties,
                  node->id(),
                  _1,
                  _2));

    return true;
}

}  // namespace tibee
