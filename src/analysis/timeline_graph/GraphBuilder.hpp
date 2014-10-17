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

#include "analysis/StateTimers.hpp"
#include "analysis/timeline_graph/TimelineGraph.hpp"
#include "analysis/timeline_graph/TimelineGraphProperties.hpp"
#include "base/BasicTypes.hpp"
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

    void SetTimestamp(timestamp_t ts);

    bool AddGraph(TaskId task_id, const std::string& description);
    bool AddChildTask(TaskId parent_task_id, TaskId child_task_id);
    bool AddTaskStep(TaskId task_id);
    bool EndTask(TaskId task_id);

    bool StartTimer(TaskId task_id, quark::Quark state);
    bool StopTimer(TaskId task_id, quark::Quark state);
    uint64_t ReadTimer(TaskId task_id, quark::Quark state);

    bool IncrementProperty(TaskId task_id, quark::Quark property, uint64_t increment);
    bool SetProperty(TaskId task_id, quark::Quark property, value::Value::UP value);
    value::Value* GetProperty(TaskId task_id, quark::Quark property);

    Graphs::const_iterator begin() const { return _graphs.begin(); }
    Graphs::const_iterator end() const { return _graphs.end(); }

    bool HasNodeForTaskId(TaskId task_id) const;

private:
    bool GetLastNodeForTask(TaskId task_id,
                            size_t* graph_index,
                            Node** node);

    bool ReadAndResetTimers(TaskId task_id);

    // Keeps track of the last node added to the graph for each task id.
    struct NodeKey {
        NodeKey()
            : graph_index(-1), node_id(-1) {}
        NodeKey(size_t graph_index, NodeId node_id)
            : graph_index(graph_index), node_id(node_id) {}

      size_t graph_index;
      NodeId node_id;
    };
    typedef std::unordered_map<TaskId, NodeKey> TaskIdNodeKeyMap;
    TaskIdNodeKeyMap _last_node_for_task_id;

    // The constructed graphs.
    Graphs _graphs;

    // Current timestamp.
    timestamp_t _ts;

    // Timers.
    typedef std::unordered_map<TaskId, StateTimers> TaskIdToStateTimers;
    TaskIdToStateTimers _timers;
};

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee

#endif // _GRAPHBUILDER_GRAPHBUILDER_HPP
