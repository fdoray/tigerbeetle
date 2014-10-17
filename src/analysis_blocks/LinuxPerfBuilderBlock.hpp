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
#ifndef _TIBEE_ANALYSISBLOCKS_LINUXPERFBUILDERBLOCK_HPP
#define _TIBEE_ANALYSISBLOCKS_LINUXPERFBUILDERBLOCK_HPP

#include "analysis/timeline_graph/GraphBuilder.hpp"
#include "base/BasicTypes.hpp"
#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"
#include "state/AttributeKey.hpp"
#include "state/CurrentState.hpp"

namespace tibee {
namespace analysis_blocks {

/**
 * Linux perf builder block.
 *
 * @author Francois Doray
 */
class LinuxPerfBuilderBlock : public block::AbstractBlock
{
public:
    LinuxPerfBuilderBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onEvent(const notification::Path& path, const value::Value* value);

    void IncrementPerfCounter(uint32_t cpu, int32_t thread, quark::Quark counter, const value::Value* value);

    // Current state.
    state::CurrentState* _currentState;

    // Graph builder.
    analysis::timeline_graph::GraphBuilder* _graphBuilder;

    // Quarks.
    quark::Quark Q_INSTRUCTIONS;
    quark::Quark Q_CACHE_REFERENCES;
    quark::Quark Q_CACHE_MISSES;
    quark::Quark Q_BRANCH_INSTRUCTIONS;
    quark::Quark Q_BRANCHES;
    quark::Quark Q_BRANCH_MISSES;
    quark::Quark Q_BRANCH_LOADS;
    quark::Quark Q_BRANCH_LOAD_MISSES;
    quark::Quark Q_PAGE_FAULT;
    quark::Quark Q_FAULTS;
    quark::Quark Q_MAJOR_FAULTS;
    quark::Quark Q_MINOR_FAULTS;

    quark::Quark Q_CUR_THREAD;
    quark::Quark Q_STATUS;
    quark::Quark Q_INTERRUPTED;

    // CPUs attribute.
    state::AttributeKey _cpusAttribute;

    // Threads attribute.
    state::AttributeKey _threadsAttribute;

    // Last value read for each performance counter, per CPU.
    typedef std::vector<std::unordered_map<quark::Quark, uint64_t>> PerfCounters;
    PerfCounters _perfCounters;
};

}  // namespace analysis_blocks
}  // namespace tibee

#endif // _TIBEE_ANALYSISBLOCKS_LINUXGRAPHBUILDERBLOCK_HPP
