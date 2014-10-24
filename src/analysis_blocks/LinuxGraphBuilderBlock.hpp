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
#ifndef _TIBEE_ANALYSISBLOCKS_LINUXGRAPHBUILDERBLOCK_HPP
#define _TIBEE_ANALYSISBLOCKS_LINUXGRAPHBUILDERBLOCK_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "analysis/execution_graph/GraphBuilder.hpp"
#include "base/BasicTypes.hpp"
#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"
#include "state/CurrentState.hpp"

namespace tibee {
namespace analysis_blocks {

/**
 * Linux graph builder block.
 *
 * @author Francois Doray
 */
class LinuxGraphBuilderBlock : public block::AbstractBlock
{
public:
    LinuxGraphBuilderBlock();

    virtual void Start(const value::Value* parameters) override;
    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onExecName(const notification::Path& path, const value::Value* value);
    void onSchedProcessFork(const notification::Path& path, const value::Value* value);
    void onSchedProcessExit(const notification::Path& path, const value::Value* value);
    void onStatusChange(const notification::Path& path, const value::Value* value);
    void onSyscallChange(const notification::Path& path, const value::Value* value);

    typedef std::unordered_set<std::string> AnalyzedExecutables;
    AnalyzedExecutables _analyzedExecutables;

    // Current state.
    state::CurrentState* _currentState;

    // Graph builder.
    analysis::execution_graph::GraphBuilder* _graphBuilder;

    // Set of task ids that haven't been executed yet.
    typedef std::unordered_set<analysis::execution_graph::GraphBuilder::TaskId> PendingTasks;
    PendingTasks _pendingTasks;

    // Quarks.
    quark::Quark Q_LINUX;
    quark::Quark Q_THREADS;
    quark::Quark Q_SYSCALL;
    quark::Quark Q_STATUS;
    quark::Quark Q_WAIT_FOR_CPU;
    quark::Quark Q_PPID;
    quark::Quark Q_DURATION;
    quark::Quark Q_NODE_TYPE;
};

}  // namespace analysis_blocks
}  // namespace tibee

#endif // _TIBEE_ANALYSISBLOCKS_LINUXGRAPHBUILDERBLOCK_HPP
