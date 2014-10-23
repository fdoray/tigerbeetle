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
#ifndef _TIBEE_ANALYSIS_EXECUTIONGRAPH_GRAPHBUILDER_HPP
#define _TIBEE_ANALYSIS_EXECUTIONGRAPH_GRAPHBUILDER_HPP

#include <memory>
#include <unordered_map>
#include <stack>

#include "analysis/StateTimers.hpp"
#include "analysis/execution_graph/Graph.hpp"
#include "analysis/execution_graph/Properties.hpp"
#include "base/BasicTypes.hpp"
#include "quark/StringQuarkDatabase.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

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
        Graph graph;
        Properties properties;
    };
    typedef std::vector<GraphAndProperties::UP> Graphs;

    GraphBuilder();
    ~GraphBuilder();

    void SetQuarks(quark::StringQuarkDatabase* quarks);

    void SetTimestamp(timestamp_t ts);

    // Graph structure.
    bool CreateGraph(ThreadId thread, TaskId task, const std::string& description);
    bool PushStack(ThreadId thread);
    bool PopStack(ThreadId thread);
    bool CreateTask(ThreadId parent_thread, TaskId child_task);
    bool ScheduleTask(TaskId task, ThreadId thread);

    // Timers.
    bool StartTimer(ThreadId thread, quark::Quark state);
    bool StopTimer(ThreadId thread, quark::Quark state);
    uint64_t ReadTimer(ThreadId thread, quark::Quark state);
    void StopAllTimers();

    // Properties.
    bool IncrementProperty(ThreadId thread, quark::Quark property, uint64_t increment);
    bool SetProperty(ThreadId thread, quark::Quark property, value::Value::UP value);
    const value::Value* GetProperty(ThreadId thread, quark::Quark property);

    // Iteration on constructed graphs.
    Graphs::const_iterator begin() const { return _graphs.begin(); }
    Graphs::const_iterator end() const { return _graphs.end(); }

private:
    bool ReadAndResetTimers(ThreadId thread);

    // The constructed graphs.
    Graphs _graphs;

    // Current timestamp.
    timestamp_t _ts;

    // Active timers.
    typedef std::unordered_map<ThreadId, StateTimers> ThreadIdToStateTimers;
    ThreadIdToStateTimers _timers;

    // Stacks for active tasks.
    typedef std::unordered_map<TaskId, std::stack<Node*>> TaskStacks;
    TaskStacks _taskStacks;

    // Tasks scheduled on threads.
    typedef std::unordered_map<ThreadId, TaskId> ThreadTasks;
    ThreadTasks _threadTasks;

    // Map of task id to graph index.
    typedef std::unordered_map<TaskId, size_t> TaskGraphIndex;
    TaskGraphIndex _taskGraphIndex;

    // Quarks.
    quark::Quark Q_PARENT_TID;
    quark::Quark Q_TID;
    quark::Quark Q_DURATION;
    quark::Quark Q_NODE_TYPE;
    quark::Quark Q_START_TIME;
    quark::Quark Q_STACK_DEPTH;
    quark::Quark Q_ARROW_START;
};

}    // namespace execution_graph
}    // namespace analysis
}    // namespace tibee

#endif // _GRAPHBUILDER_GRAPHBUILDER_HPP
