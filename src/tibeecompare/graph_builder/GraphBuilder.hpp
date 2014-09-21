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
#ifndef _GRAPHBUILDER_GRAPHBUILDER_HPP
#define _GRAPHBUILDER_GRAPHBUILDER_HPP

#include <boost/unordered_map.hpp>
#include <memory>

#include <common/BasicTypes.hpp>
#include "GraphAndProperties.hpp"
#include "TaskTimers.hpp"

namespace tibee
{

/**
 * Builds a graph representing an execution.
 *
 * @author Francois Doray
 */
class GraphBuilder
{
public:
    typedef uint64_t TaskId;
    typedef std::vector<GraphAndProperties::UP> Graphs;
    typedef std::unique_ptr<Graphs> GraphsUP;

    GraphBuilder();
    ~GraphBuilder();

    void Reset();

    GraphsUP TakeGraphs();

    void SetTimestamp(common::timestamp_t ts);

    bool AddGraph(TaskId task_id);
    bool AddTask(TaskId parent_task_id, TaskId child_task_id);
    bool AddTaskStep(TaskId task_id);
    bool EndTask(TaskId task_id);

    bool StartTimer(TaskId task_id, const std::string& timer_name);
    bool StopTimer(TaskId task_id, const std::string& timer_name);

private:
    bool GetLastNodeForTask(TaskId task_id,
                            size_t* graph_index,
                            GraphAndProperties** graph,
                            timeline_graph::TimelineNode** node);

    bool ReadAndResetTimers(TaskId task_id);

    // Keeps track of the last node added to the graph for each task id.
    struct NodeKey {
        NodeKey()
            : graph_index(-1), node_id(-1) {}
        NodeKey(size_t graph_index, timeline_graph::TimelineNodeId node_id)
            : graph_index(graph_index), node_id(node_id) {}

      size_t graph_index;
      timeline_graph::TimelineNodeId node_id;
    };
    typedef boost::unordered_map<TaskId, NodeKey> TaskIdNodeKeyMap;
    TaskIdNodeKeyMap _last_node_for_task_id;

    // The constructed graphs.
    GraphsUP _graphs;

    // Current timestamp.
    common::timestamp_t _ts;

    // Timers.
    typedef boost::unordered_map<TaskId, TaskTimers> TasksTimers;
    TasksTimers _timers;
};

}  // namespace tibee

#endif // _GRAPHBUILDER_GRAPHBUILDER_HPP
