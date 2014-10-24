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
#ifndef _TIBEE_ANALYSISBLOCKS_CHROMEGRAPHBUILDERBLOCK_HPP
#define _TIBEE_ANALYSISBLOCKS_CHROMEGRAPHBUILDERBLOCK_HPP

#include <string>
#include <unordered_set>

#include "analysis/execution_graph/GraphBuilder.hpp"
#include "base/BasicTypes.hpp"
#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"
#include "state/CurrentState.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee {
namespace analysis_blocks {

/**
 * Chrome graph builder block.
 *
 * @author Francois Doray
 */
class ChromeGraphBuilderBlock : public block::AbstractBlock
{
public:
    ChromeGraphBuilderBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onExecName(const notification::Path& path, const value::Value* value);
    void onChromeTracing(const notification::Path& path, const value::Value* value);
    void onStatusChange(const notification::Path& path, const value::Value* value);
    void onSyscallChange(const notification::Path& path, const value::Value* value);
    void onSchedProcessFork(const notification::Path& path, const value::Value* value);

    void onPhaseBegin(uint32_t tid, const trace::EventValue& event);
    void onPhaseEnd(uint32_t tid, const trace::EventValue& event);
    void onPhaseFlowBegin(uint32_t tid, const trace::EventValue& event);
    void onPhaseFlowEnd(uint32_t tid, const trace::EventValue& event);

    uint32_t currentThreadForCpu(uint32_t cpu);

    // Current state.
    state::CurrentState* _currentState;

    // Graph builder.
    analysis::execution_graph::GraphBuilder* _graphBuilder;

    // Quarks.
    quark::Quark Q_LINUX;
    quark::Quark Q_THREADS;
    quark::Quark Q_SYSCALL;
    quark::Quark Q_STATUS;
    quark::Quark Q_WAIT_FOR_CPU;
    quark::Quark Q_CUR_THREAD;
    quark::Quark Q_PPID;
    quark::Quark Q_DURATION;
    quark::Quark Q_NODE_TYPE;

    // CPUs attribute.
    state::AttributeKey _cpusAttribute;

    // Threads that belong to analyzed executables.
    std::unordered_set<uint32_t> _analyzedThreads;

    // Counters to track lost UST events. TODO(fdoray)
    std::unordered_map<uint64_t, uint64_t> _counters;
};

}  // namespace analysis_blocks
}  // namespace tibee

#endif // _TIBEE_ANALYSISBLOCKS_CHROMEGRAPHBUILDERBLOCK_HPP
