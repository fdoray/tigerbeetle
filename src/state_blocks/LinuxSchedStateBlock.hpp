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
#ifndef _TIBEE_STATEBLOCKS_LINUXSCHEDSTATEBLOCK_HPP
#define _TIBEE_STATEBLOCKS_LINUXSCHEDSTATEBLOCK_HPP

#include "quark/Quark.hpp"
#include "state/CurrentState.hpp"
#include "state_blocks/AbstractStateBlock.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee
{
namespace state_blocks
{

/**
 * A block that produces state changes from Linux scheduling events.
 *
 * @author Francois Doray
 */
class LinuxSchedStateBlock : public AbstractStateBlock
{
public:
    LinuxSchedStateBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onSysEntryExecve(const trace::EventValue& event);
    void onExitSyscall(const trace::EventValue& event);
    void onIrqHandlerEntry(const trace::EventValue& event);
    void onIrqHandlerExit(const trace::EventValue& event);
    void onSoftIrqEntry(const trace::EventValue& event);
    void onSoftIrqExit(const trace::EventValue& event);
    void onSoftIrqRaise(const trace::EventValue& event);
    void onSchedSwitch(const trace::EventValue& event);
    void onSchedProcessFork(const trace::EventValue& event);
    void onSchedProcessFree(const trace::EventValue& event);
    void onLttngStatedumpProcessState(const trace::EventValue& event);
    void onSchedWakeupEvent(const trace::EventValue& event);
    void onSysEvent(const trace::EventValue& event);

    // Utility methods.
    uint32_t getEventCpu(const trace::EventValue& event) const;
    quark::Quark getEventCpuQuark(const trace::EventValue& event) const;
    state::AttributeKey getLinuxAttribute() const;
    state::AttributeKey getCurrentCpuAttribute(const trace::EventValue& event) const;
    state::AttributeKey getCpuCurrentThreadAttribute(const trace::EventValue& event) const;
    state::AttributeKey getCurrentThreadAttribute(const trace::EventValue& event) const;
    state::AttributeKey getCurrentIrqAttribute(const trace::EventValue& event) const;
    state::AttributeKey getCurrentSoftIrqAttribute(const trace::EventValue& event) const;

    // Quarks.
    quark::Quark Q_LINUX;
    quark::Quark Q_THREADS;
    quark::Quark Q_CPUS;
    quark::Quark Q_CUR_CPU;
    quark::Quark Q_CUR_THREAD;
    quark::Quark Q_RESOURCES;
    quark::Quark Q_IRQS;
    quark::Quark Q_SOFT_IRQS;
    quark::Quark Q_SYSCALL;
    quark::Quark Q_STATUS;
    quark::Quark Q_PPID;
    quark::Quark Q_EXEC_NAME;
    quark::Quark Q_IDLE;
    quark::Quark Q_RUN_USERMODE;
    quark::Quark Q_RUN_SYSCALL;
    quark::Quark Q_IRQ;
    quark::Quark Q_SOFT_IRQ;
    quark::Quark Q_UNKNOWN;
    quark::Quark Q_WAIT_BLOCKED;
    quark::Quark Q_INTERRUPTED;
    quark::Quark Q_WAIT_FOR_CPU;
    quark::Quark Q_RAISED;
};

}
}

#endif // _TIBEE_STATEBLOCKS_LINUXSCHEDSTATEBLOCK_HPP
