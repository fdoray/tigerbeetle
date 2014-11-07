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
#ifndef _TIBEE_EXECUTION_EXECUTIONBUILDER_HPP
#define _TIBEE_EXECUTION_EXECUTIONBUILDER_HPP

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <stack>

#include "execution/StateTimers.hpp"
#include "execution/Graph.hpp"
#include "execution/Properties.hpp"
#include "base/BasicTypes.hpp"
#include "quark/StringQuarkDatabase.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace execution {

/**
 * Builds graphs representing the execution of tasks.
 *
 * @author Francois Doray
 */
class ExecutionBuilder
{
public:
    typedef uint64_t TaskId;
    typedef uint32_t ThreadId;

    struct Execution
    {
        typedef std::unique_ptr<Execution> UP;
        std::string description;
        Graph graph;
        Properties properties;
    };
    typedef std::vector<Execution::UP> Executions;

    ExecutionBuilder();
    ~ExecutionBuilder();

    void SetQuarks(quark::StringQuarkDatabase* quarks);

    void SetTimestamp(timestamp_t ts);

    // Execution structure.
    bool CreateExecution(ThreadId thread, const std::string& description);
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

    // Iteration on constructed executions.
    Executions::const_iterator begin() const { return _executions.begin(); }
    Executions::const_iterator end() const { return _executions.end(); }

    // Accessors.
    bool HasNodeForThread(ThreadId thread) const { return _threadTasks.find(thread) != _threadTasks.end(); }

private:
    bool ReadAndResetTimers(ThreadId thread);

    // The constructed graphs.
    Executions _executions;

    // Current timestamp.
    timestamp_t _ts;

    // Active timers.
    typedef std::unordered_map<ThreadId, StateTimers> ThreadIdToStateTimers;
    ThreadIdToStateTimers _timers;

    // Stacks for pending tasks.
    typedef std::unordered_map<TaskId, std::stack<Node*>> TaskStacks;
    TaskStacks _pendingTasks;

    // Stacks for scheduled tasks.
    TaskStacks _scheduledTasks;

    // Tasks scheduled on threads.
    typedef std::unordered_map<ThreadId, std::stack<TaskId>> ThreadTasks;
    ThreadTasks _threadTasks;

    // Stack depth per thread.
    typedef std::unordered_map<ThreadId, uint32_t> StackDepthMap;
    StackDepthMap _stackDepthMap;

    // Map of task id to graph index.
    typedef std::unordered_map<TaskId, size_t> TaskGraphIndex;
    TaskGraphIndex _taskGraphIndex;

    // Internal task counter.
    TaskId _taskCounter;

    // Quarks.
    quark::Quark Q_PARENT_TID;
    quark::Quark Q_TID;
    quark::Quark Q_DURATION;
    quark::Quark Q_NODE_TYPE;
    quark::Quark Q_START_TIME;
    quark::Quark Q_STACK_DEPTH;
    quark::Quark Q_ARROW_START;
};

}    // namespace execution
}    // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTIONBUILDER_HPP
