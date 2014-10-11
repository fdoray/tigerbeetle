/* This is a simple Linux kernel state provider which records the state
 * of processes and threads. It is based on the work of Florian Wininger,
 * reimplementing his XML state machine using tigerbeetle.
 *
 * His XML state machine was found at:
 *
 *   <http://secretaire.dorsal.polymtl.ca/~fwininger/XmlStateProvider/kernel-0.6.state-schema.xml>
 *
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
 * Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "state_blocks/LinuxSchedStateBlock.hpp"

#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "value/MakeValue.hpp"

#include <iostream>

namespace tibee
{
namespace state_blocks
{

namespace
{

using notification::RegexToken;
using notification::Token;
using value::MakeValue;

const char kSyscallWithParamsPrefix[] = "syscall_entry_";

}  // namespace

LinuxSchedStateBlock::LinuxSchedStateBlock()
{
}

void LinuxSchedStateBlock::LoadServices(const block::ServiceList& serviceList)
{
    AbstractStateBlock::LoadServices(serviceList);

    // Get constant quarks.
    Q_LINUX = State()->Quark("linux");
    Q_THREADS = State()->Quark("threads");
    Q_CPUS = State()->Quark("cpus");
    Q_CUR_CPU = State()->Quark("cur-cpu");
    Q_CUR_THREAD = State()->Quark("cur-thread");
    Q_RESOURCES = State()->Quark("resources");
    Q_IRQS = State()->Quark("irqs");
    Q_SOFT_IRQS = State()->Quark("soft-irqs");
    Q_SYSCALL = State()->Quark("syscall");
    Q_STATUS = State()->Quark("status");
    Q_PPID = State()->Quark("ppid");
    Q_EXEC_NAME = State()->Quark("exec-name");
    Q_IDLE = State()->Quark("idle");
    Q_RUN_USERMODE = State()->Quark("usermode");
    Q_RUN_SYSCALL = State()->Quark("syscall");
    Q_IRQ = State()->Quark("irq");
    Q_SOFT_IRQ = State()->Quark("soft-irq");
    Q_UNKNOWN = State()->Quark("unknown");
    Q_WAIT_BLOCKED = State()->Quark("wait-blocked");
    Q_INTERRUPTED = State()->Quark("interrupted");
    Q_WAIT_FOR_CPU = State()->Quark("wait-for-cpu");
    Q_RAISED = State()->Quark("raised");
    Q_SYS_CLONE = State()->Quark("sys_clone");
}

void LinuxSchedStateBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    namespace pl = std::placeholders;

    AddKernelObserver(notificationCenter, Token("syscall_entry_execve"), std::bind(&LinuxSchedStateBlock::onSysEntryExecve, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("exit_syscall"), std::bind(&LinuxSchedStateBlock::onExitSyscall, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("irq_handler_entry"), std::bind(&LinuxSchedStateBlock::onIrqHandlerEntry, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("irq_handler_exit"), std::bind(&LinuxSchedStateBlock::onIrqHandlerExit, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("softirq_entry"), std::bind(&LinuxSchedStateBlock::onSoftIrqEntry, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("softirq_exit"), std::bind(&LinuxSchedStateBlock::onSoftIrqExit, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("softirq_raise"), std::bind(&LinuxSchedStateBlock::onSoftIrqRaise, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("sched_switch"), std::bind(&LinuxSchedStateBlock::onSchedSwitch, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("sched_process_fork"), std::bind(&LinuxSchedStateBlock::onSchedProcessFork, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("sched_process_free"), std::bind(&LinuxSchedStateBlock::onSchedProcessFree, this, pl::_1));
    AddKernelObserver(notificationCenter, Token("lttng_statedump_process_state"), std::bind(&LinuxSchedStateBlock::onLttngStatedumpProcessState, this, pl::_1));
    AddKernelObserver(notificationCenter, RegexToken("^sched_wakeup"), std::bind(&LinuxSchedStateBlock::onSchedWakeupEvent, this, pl::_1));
    AddKernelObserver(notificationCenter, RegexToken("^sys_"), std::bind(&LinuxSchedStateBlock::onSysEvent, this, pl::_1));
    AddKernelObserver(notificationCenter, RegexToken("^compat_sys_"), std::bind(&LinuxSchedStateBlock::onSysEvent, this, pl::_1));
    AddKernelObserver(notificationCenter, RegexToken("^syscall_entry_"), std::bind(&LinuxSchedStateBlock::onSysEvent, this, pl::_1));
    AddKernelObserver(notificationCenter, RegexToken("^syscall_exit_"), std::bind(&LinuxSchedStateBlock::onExitSyscall, this, pl::_1));
}

void LinuxSchedStateBlock::onSysEntryExecve(const trace::EventValue& event)
{
    auto currentThreadAttribute = getCurrentThreadAttribute(event);
    auto filename = event.getFields()->GetField("filename")->AsString();
    auto last_slash_pos = filename.find_last_of('/');
    if (last_slash_pos != std::string::npos)
        filename = filename.substr(last_slash_pos + 1);

    // exec name
    State()->SetAttribute(currentThreadAttribute, {Q_EXEC_NAME}, MakeValue(filename));
}

void LinuxSchedStateBlock::onExitSyscall(const trace::EventValue& event)
{
    auto currentThreadAttribute = getCurrentThreadAttribute(event);
    auto currentCpuAttribute = getCurrentCpuAttribute(event);

    // current thread status
    if (currentThreadAttribute != state::InvalidAttributeKey())
    {
        State()->NullAttribute(currentThreadAttribute, {Q_SYSCALL});
        State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
    }

    // current CPU status
    State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
}

void LinuxSchedStateBlock::onIrqHandlerEntry(const trace::EventValue& event)
{
    auto currentThreadAttribute = getCurrentThreadAttribute(event);
    auto currentCpuAttribute = getCurrentCpuAttribute(event);
    auto currentIrqAttribute = getCurrentIrqAttribute(event);
    auto cpu = getEventCpu(event);

    // current IRQ's CPU
    State()->SetAttribute(currentIrqAttribute, {Q_CUR_CPU}, MakeValue(cpu));

    if (currentThreadAttribute != state::InvalidAttributeKey())
    {
        // current thread's status
        State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_INTERRUPTED));
    }

    // current CPU's status
    State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_IRQ));
}

void LinuxSchedStateBlock::onIrqHandlerExit(const trace::EventValue& event)
{
    auto currentThreadAttribute = getCurrentThreadAttribute(event);
    auto currentCpuAttribute = getCurrentCpuAttribute(event);
    auto cpuCurrentThreadAttribute = getCpuCurrentThreadAttribute(event);
    auto currentIrqAttribute = getCurrentIrqAttribute(event);

    // reset current IRQ's CPU
    State()->NullAttribute(currentIrqAttribute, {Q_CUR_CPU});

    bool cpuIsIdle =
        State()->GetAttributeValue(cpuCurrentThreadAttribute) == nullptr ||
        State()->GetAttributeValue(cpuCurrentThreadAttribute)->AsInteger() == 0;

    if (currentThreadAttribute != state::InvalidAttributeKey() && !cpuIsIdle)
    {
        if (State()->GetAttributeValue(currentThreadAttribute, {Q_SYSCALL}) == nullptr)
        {
            // syscall not set for current thread: running in usermode
            State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
            State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
        }
        else
        {
            // syscall set for current thread: running a syscall
            State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
            State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
        }
    }

    if (cpuIsIdle)
    {
        // no current thread for this CPU: CPU is idle.
        State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_IDLE));
    }
}

void LinuxSchedStateBlock::onSoftIrqEntry(const trace::EventValue& event)
{
    auto currentThreadAttribute = getCurrentThreadAttribute(event);
    auto currentCpuAttribute = getCurrentCpuAttribute(event);
    auto currentSoftIrqAttribute = getCurrentSoftIrqAttribute(event);
    auto cpu = getEventCpu(event);

    // current soft IRQ's CPU
    State()->SetAttribute(currentSoftIrqAttribute, {Q_CUR_CPU}, MakeValue(cpu));

    // reset current soft IRQ's CPU
    State()->NullAttribute(currentSoftIrqAttribute, {Q_STATUS});

    if (currentThreadAttribute != state::InvalidAttributeKey())
    {
        // current thread's status
        State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_INTERRUPTED));
    }

    // current CPU's status
    State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_SOFT_IRQ));
}

void LinuxSchedStateBlock::onSoftIrqExit(const trace::EventValue& event)
{
    auto currentThreadAttribute = getCurrentThreadAttribute(event);
    auto currentCpuAttribute = getCurrentCpuAttribute(event);
    auto cpuCurrentThreadAttribute = getCpuCurrentThreadAttribute(event);
    auto currentSoftIrqAttribute = getCurrentSoftIrqAttribute(event);

    // reset current soft IRQ's CPU
    State()->NullAttribute(currentSoftIrqAttribute, {Q_CUR_CPU});

    // reset current soft IRQ's status
    State()->NullAttribute(currentSoftIrqAttribute, {Q_STATUS});

    bool cpuIsIdle =
        State()->GetAttributeValue(cpuCurrentThreadAttribute) == nullptr ||
        State()->GetAttributeValue(cpuCurrentThreadAttribute)->AsInteger() == 0;

    if (currentThreadAttribute != state::InvalidAttributeKey() && !cpuIsIdle)
    {
        if (State()->GetAttributeValue(currentThreadAttribute, {Q_SYSCALL}) == nullptr)
        {
            // syscall not set for current thread: running in usermode
            State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
            State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
        }
        else
        {
            // syscall set for current thread: running a syscall
            State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
            State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
        }
    }

    if (cpuIsIdle)
    {
        // no current thread for this CPU: CPU is idle.
        State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_IDLE));
    }
}

void LinuxSchedStateBlock::onSoftIrqRaise(const trace::EventValue& event)
{
    auto currentSoftIrqAttribute = getCurrentSoftIrqAttribute(event);

    // current soft IRQ's status: raised
    State()->SetAttribute(currentSoftIrqAttribute, {Q_STATUS}, MakeValue(Q_RAISED));
}

void LinuxSchedStateBlock::onSchedSwitch(const trace::EventValue& event)
{
    auto linuxAttribute = getLinuxAttribute();
    auto prevState = event.getFields()->GetField("prev_state")->AsInteger();
    auto prevTid = event.getFields()->GetField("prev_tid")->AsInteger();
    auto qPrevTid = State()->IntQuark(prevTid);
    auto nextTid =  event.getFields()->GetField("next_tid")->AsInteger();
    auto qNextTid =  State()->IntQuark(nextTid);
    auto nextComm = event.getFields()->GetField("next_comm")->AsString();
    auto currentCpuAttribute = getCurrentCpuAttribute(event);
    auto threadsPrevTidStatusAttribute =
        State()->GetAttributeKey(linuxAttribute, {Q_THREADS, qPrevTid, Q_STATUS});

    if (prevState == 0) {
        State()->SetAttribute(threadsPrevTidStatusAttribute, MakeValue(Q_WAIT_FOR_CPU));
    } else {
        State()->SetAttribute(threadsPrevTidStatusAttribute, MakeValue(Q_WAIT_BLOCKED));
    }

    auto newCurrentThread =
        State()->GetAttributeKey(linuxAttribute, {Q_THREADS, qNextTid});

    // new current thread's run mode
    if (State()->GetAttributeValue(newCurrentThread, {Q_SYSCALL}) == nullptr) {
        State()->SetAttribute(newCurrentThread, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
    } else {
        State()->SetAttribute(newCurrentThread, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
    }

    // thread's exec name
    State()->SetAttribute(newCurrentThread, {Q_EXEC_NAME}, MakeValue(nextComm));

    // current CPU's current thread
    State()->SetAttribute(currentCpuAttribute, {Q_CUR_THREAD}, MakeValue(nextTid));

    // current CPU's status
    if (nextTid != 0L) {
        if (State()->GetAttributeValue(newCurrentThread, {Q_SYSCALL}) != nullptr) {
            State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
        } else {
            State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_USERMODE));
        }
    } else {
        State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_IDLE));
    }
}

void LinuxSchedStateBlock::onSchedProcessFork(const trace::EventValue& event)
{
    auto linuxAttribute = getLinuxAttribute();
    auto childTid = event.getFields()->GetField("child_tid")->AsInteger();
    auto qChildTid = State()->IntQuark(childTid);
    auto parentTid = event.getFields()->GetField("parent_tid")->AsInteger();
    auto qParentTid = State()->IntQuark(parentTid);
    auto childComm = event.getFields()->GetField("child_comm")->AsString();
    auto threadsChildTidAttribute =
        State()->GetAttributeKey(linuxAttribute, {Q_THREADS, qChildTid});

    // child thread's parent TID
    State()->SetAttribute(threadsChildTidAttribute, {Q_PPID}, MakeValue(parentTid));

    // child thread's exec name
    State()->SetAttribute(threadsChildTidAttribute, {Q_EXEC_NAME}, MakeValue(childComm));

    // child thread's status
    State()->SetAttribute(threadsChildTidAttribute, {Q_STATUS}, MakeValue(Q_WAIT_FOR_CPU));

    // child thread's syscall
    auto parentSyscall =
        State()->GetAttributeValue(linuxAttribute, {Q_THREADS, qParentTid, Q_SYSCALL});
    if (parentSyscall) {
        State()->SetAttribute(threadsChildTidAttribute, {Q_SYSCALL}, parentSyscall->Copy());
    }

    if (State()->GetAttributeValue(threadsChildTidAttribute, {Q_SYSCALL}) == nullptr) {
        State()->SetAttribute(threadsChildTidAttribute, {Q_SYSCALL}, MakeValue(Q_SYS_CLONE));
    }
}

void LinuxSchedStateBlock::onSchedProcessFree(const trace::EventValue& event)
{
    auto linuxAttribute = getLinuxAttribute();
    auto qTid = State()->IntQuark(event.getFields()->GetField("tid")->AsInteger());

    // nullify thread subtree
    State()->NullAttribute(linuxAttribute, {Q_THREADS, qTid});
}

void LinuxSchedStateBlock::onLttngStatedumpProcessState(const trace::EventValue& event)
{
    auto linuxAttribute = getLinuxAttribute();
    auto qTid = State()->IntQuark(event.getFields()->GetField("tid")->AsInteger());
    auto ppid = event.getFields()->GetField("ppid")->AsInteger();
    auto status = event.getFields()->GetField("status")->AsInteger();
    auto name = event.getFields()->GetField("name")->AsString();
    auto threadsTidAttribute = State()->GetAttributeKey(linuxAttribute, {Q_THREADS, qTid});
    auto threadsTidExecNameAttribute = State()->GetAttributeKey(threadsTidAttribute, {Q_EXEC_NAME});
    auto threadsTidPpidAttribute = State()->GetAttributeKey(threadsTidAttribute, {Q_PPID});
    auto threadsTidStatusAttribute = State()->GetAttributeKey(threadsTidAttribute, {Q_STATUS});

    // initialize thread's exec name
    if (State()->GetAttributeValue(threadsTidExecNameAttribute) == nullptr) {
        State()->SetAttribute(threadsTidExecNameAttribute, MakeValue(name));
    }

    // initialize thread's parent TID
    if (State()->GetAttributeValue(threadsTidPpidAttribute) == nullptr) {
        State()->SetAttribute(threadsTidPpidAttribute, MakeValue(ppid));
    }

    // initialize thread's status
    if (State()->GetAttributeValue(threadsTidStatusAttribute) == nullptr) {
        if (status == 2L) {
            State()->SetAttribute(threadsTidStatusAttribute, MakeValue(Q_WAIT_FOR_CPU));
        } else if (status == 5L) {
            State()->SetAttribute(threadsTidStatusAttribute, MakeValue(Q_WAIT_BLOCKED));
        } else {
            State()->SetAttribute(threadsTidStatusAttribute, MakeValue(Q_UNKNOWN));
        }   
    }
}

void LinuxSchedStateBlock::onSchedWakeupEvent(const trace::EventValue& event)
{
    auto linuxAttribute = getLinuxAttribute();
    auto qTid = State()->IntQuark(event.getFields()->GetField("tid")->AsInteger());
    auto threadsTidStatusAttribute = State()->GetAttributeKey(
        linuxAttribute, {Q_THREADS, qTid, Q_STATUS});

    if (State()->GetAttributeValue(threadsTidStatusAttribute) != nullptr)
    {
        auto qThreadTidStatusAttribute =
            State()->GetAttributeValue(threadsTidStatusAttribute)->AsUInteger();
        if (qThreadTidStatusAttribute != Q_RUN_USERMODE.get() &&
            qThreadTidStatusAttribute != Q_RUN_SYSCALL.get()) {
            State()->SetAttribute(threadsTidStatusAttribute, MakeValue(Q_WAIT_FOR_CPU));
        }
    }
    else
    {
        // TODO: is this right?
        State()->SetAttribute(threadsTidStatusAttribute, MakeValue(Q_WAIT_FOR_CPU));
    }
}

void LinuxSchedStateBlock::onSysEvent(const trace::EventValue& event)
{
    auto currentThreadAttribute = getCurrentThreadAttribute(event);
    auto currentCpuAttribute = getCurrentCpuAttribute(event);

    if (currentThreadAttribute != state::InvalidAttributeKey()) {
        std::string syscall = event.getName();
        if (syscall.find(kSyscallWithParamsPrefix) == 0)
            syscall = syscall.substr(strlen(kSyscallWithParamsPrefix));

        State()->SetAttribute(currentThreadAttribute, {Q_SYSCALL}, MakeValue(syscall));
        State()->SetAttribute(currentThreadAttribute, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
    }

    State()->SetAttribute(currentCpuAttribute, {Q_STATUS}, MakeValue(Q_RUN_SYSCALL));
}

uint32_t LinuxSchedStateBlock::getEventCpu(const trace::EventValue& event) const
{
    assert(event.getStreamPacketContext());
    return event.getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
}

quark::Quark LinuxSchedStateBlock::getEventCpuQuark(const trace::EventValue& event) const
{
    return State()->IntQuark(static_cast<int>(getEventCpu(event)));
}

state::AttributeKey LinuxSchedStateBlock::getLinuxAttribute() const
{
    return State()->GetAttributeKey({Q_LINUX});
}

state::AttributeKey LinuxSchedStateBlock::getCurrentCpuAttribute(const trace::EventValue& event) const
{
    auto qCpu = getEventCpuQuark(event);
    return State()->GetAttributeKey(getLinuxAttribute(), {Q_CPUS, qCpu});
}

state::AttributeKey LinuxSchedStateBlock::getCpuCurrentThreadAttribute(const trace::EventValue& event) const
{
    return State()->GetAttributeKey(getCurrentCpuAttribute(event), {Q_CUR_THREAD});
}

state::AttributeKey LinuxSchedStateBlock::getCurrentThreadAttribute(const trace::EventValue& event) const
{
    auto cpuCurrentThreadAttribute = getCpuCurrentThreadAttribute(event);
    if (State()->GetAttributeValue(cpuCurrentThreadAttribute) == nullptr)
        return state::InvalidAttributeKey();

    auto qCurrentThread = State()->IntQuark(State()->GetAttributeValue(cpuCurrentThreadAttribute)->AsInteger());

    return State()->GetAttributeKey(getLinuxAttribute(), {Q_THREADS, qCurrentThread});
}

state::AttributeKey LinuxSchedStateBlock::getCurrentIrqAttribute(const trace::EventValue& event) const
{
    int32_t irq = event.getFields()->GetField("irq")->AsInteger();
    auto qIrq = State()->IntQuark(irq);

    return State()->GetAttributeKey(getLinuxAttribute(), {Q_RESOURCES, Q_IRQS, qIrq});
}

state::AttributeKey LinuxSchedStateBlock::getCurrentSoftIrqAttribute(const trace::EventValue& event) const
{
    uint32_t vec = event.getFields()->GetField("vec")->AsUInteger();
    auto qVec = State()->IntQuark(vec);

    return State()->GetAttributeKey(getLinuxAttribute(), {Q_RESOURCES, Q_SOFT_IRQS, qVec});   
}

}
}
