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
#ifndef _TIBEE_ANALYSIS_TIMELINEGRAPH_GRAPHBUILDER_HPP
#define _TIBEE_ANALYSIS_TIMELINEGRAPH_GRAPHBUILDER_HPP

#include <memory>
#include <unordered_map>
#include <stack>

#include "analysis/StateTimers.hpp"
#include "analysis/timeline_graph/TimelineGraph.hpp"
#include "analysis/timeline_graph/TimelineGraphProperties.hpp"
#include "base/BasicTypes.hpp"
#include "quark/StringQuarkDatabase.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

/**
 * Builds graphs representing the execution of tasks.
 *
 * @author Francois Doray
 */
class GraphBuilder
{
public:
    typedef uint64_t TaskId;
    typedef uint32_t ThreadId;

    struct GraphAndProperties
    {
        typedef std::unique_ptr<GraphAndProperties> UP;
        std::string description;
        TimelineGraph graph;
        TimelineGraphProperties properties;
    };
    typedef std::vector<GraphAndProperties::UP> Graphs;

    GraphBuilder();
    ~GraphBuilder();

    void SetQuarks(quark::StringQuarkDatabase* quarks);

    void SetTimestamp(timestamp_t ts);
    timestamp_t GetTimestamp() const { return _ts; }

    bool AddGraph(ThreadId thread_id, const std::string& description);
    bool AddChildTask(ThreadId parent_thread_id, TaskId child_task_id);
    bool ScheduleTask(TaskId task_id, ThreadId thread_id);
    bool AddStepForThreadId(ThreadId thread_id);
    bool EndTaskOnThread(ThreadId thread_id);

    bool PushStack(ThreadId thread_id);
    bool PopStack(ThreadId thread_id);

    bool StartTimer(ThreadId thread_id, quark::Quark state);
    bool StopTimer(ThreadId thread_id, quark::Quark state);
    uint64_t ReadTimer(ThreadId thread_id, quark::Quark state);

    bool IncrementProperty(ThreadId thread_id, quark::Quark property, uint64_t increment);
    bool SetProperty(ThreadId thread_id, quark::Quark property, value::Value::UP value);
    value::Value* GetProperty(ThreadId thread_id, quark::Quark property);

    Graphs::const_iterator begin() const { return _graphs.begin(); }
    Graphs::const_iterator end() const { return _graphs.end(); }

    bool HasNodeForThread(ThreadId thread_id) const;

    void Terminate();

private:
    bool GetLastNodeForThread(ThreadId thread_id,
                              size_t* graph_index,
                              Node** node);

    bool ReadAndResetTimers(ThreadId thread_id);

    // Keeps track of the last node added to the graph for each task id.
    struct NodeKey {
        NodeKey()
            : graph_index(-1), node_id(-1) {}
        NodeKey(size_t graph_index, NodeId node_id)
            : graph_index(graph_index), node_id(node_id) {}

      size_t graph_index;
      NodeId node_id;
    };

    // Last node for task id.
    typedef std::unordered_map<TaskId, NodeKey> TaskIdNodeKeyMap;
    TaskIdNodeKeyMap _pending_tasks;

    // Last node for thread id.
    typedef std::unordered_map<ThreadId, NodeKey> ThreadIdNodeKeyMap;
    ThreadIdNodeKeyMap _last_node_for_thread_id;

    // Stack per thread id.
    typedef std::unordered_map<ThreadId, std::stack<std::string>> ThreadIdStackMap;
    ThreadIdStackMap _stacks;

    // The constructed graphs.
    Graphs _graphs;

    // Current timestamp.
    timestamp_t _ts;

    // Timers.
    typedef std::unordered_map<ThreadId, StateTimers> ThreadIdToStateTimers;
    ThreadIdToStateTimers _timers;

    // Quarks.
    quark::Quark Q_TID;
    quark::Quark Q_PPID;
    quark::Quark Q_DURATION;
    quark::Quark Q_NODE_TYPE;
    quark::Quark Q_START_TIME;
    quark::Quark Q_STACK_DEPTH;
};

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee

#endif // _GRAPHBUILDER_GRAPHBUILDER_HPP
