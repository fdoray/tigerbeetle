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
#ifndef _TIBEE_CRITICALBLOCKS_CRITICALBLOCK_HPP
#define _TIBEE_CRITICALBLOCKS_CRITICALBLOCK_HPP

#include <unordered_map>

#include "block/AbstractBlock.hpp"
#include "critical/CriticalGraph.hpp"
#include "quark/Quark.hpp"
#include "state/CurrentState.hpp"

namespace tibee
{
namespace critical_blocks
{

/**
 * Critical block.
 *
 * @author Francois Doray
 */
class CriticalBlock : public block::AbstractBlock
{
public:
    CriticalBlock();

    virtual void RegisterServices(block::ServiceList* serviceList) override;
    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void OnTTWU(const trace::EventValue& event);
    void OnThreadStatus(uint32_t tid, const notification::Path& path, const value::Value* value);

    void OnTTWUBetweenThreads(uint32_t source_tid, uint32_t target_tid);

    // Thread for an event.
    uint32_t ThreadForEvent(const trace::EventValue& event) const;

    // Critical graph.
    critical::CriticalGraph _graph;

    // Last state per thread. 
    std::unordered_map<uint32_t, critical::CriticalEdgeType> _lastEdgeTypePerThread;

    // Current state.
    state::CurrentState* _currentState;

    // Constant quarks.
    quark::Quark Q_RUN_USERMODE;
    quark::Quark Q_RUN_SYSCALL;
    quark::Quark Q_WAIT_BLOCKED;
    quark::Quark Q_INTERRUPTED;
    quark::Quark Q_WAIT_FOR_CPU;
    quark::Quark Q_UNKNOWN;
};

}  // namespace tibee
}  // namespace critical_blocks

#endif // _TIBEE_CRITICALBLOCKS_CRITICALBLOCK_HPP
