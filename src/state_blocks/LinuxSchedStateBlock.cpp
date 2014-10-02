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
#include "state_blocks/LinuxSchedStateBlock.hpp"

#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"

#include <iostream>

namespace tibee
{
namespace state_blocks
{

using namespace value;
using notification::RegexToken;
using notification::Token;

namespace
{

enum NotificationTypes
{
    kThreadStatusNotification = 0,
    kThreadPpidNotification,
    kThreadExecNotification,
    kThreadSyscallNotification,
    kCpuStatusNotification,
    kCpuThreadNotification,
    kIrqCpuNotification,
};

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
}

void LinuxSchedStateBlock::onExitSyscall(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onIrqHandlerEntry(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onIrqHandlerExit(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onSoftIrqEntry(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onSoftIrqExit(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onSoftIrqRaise(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onSchedSwitch(const trace::EventValue& event)
{
    State()->SetAttribute(
        State()->GetAttributeKeyStr({"test", "dummy", "toto"}),
        MakeValue<IntValue>(42));
}

void LinuxSchedStateBlock::onSchedProcessFork(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onSchedProcessFree(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onLttngStatedumpProcessState(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onSchedWakeupEvent(const trace::EventValue& event)
{
}

void LinuxSchedStateBlock::onSysEvent(const trace::EventValue& event)
{
}

}
}
