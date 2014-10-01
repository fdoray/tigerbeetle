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

using notification::RegexToken;
using notification::Token;

const char* LinuxSchedStateBlock::kThreadStatusNotification = "thread-status";
const char* LinuxSchedStateBlock::kThreadPpidNotification = "thread-ppid";
const char* LinuxSchedStateBlock::kThreadExecNotification = "thread-exec";
const char* LinuxSchedStateBlock::kThreadSyscallNotification = "thread-syscall";
const char* LinuxSchedStateBlock::kCpuStatusNotification = "cpu-status";
const char* LinuxSchedStateBlock::kCpuCurrentThreadNotification = "cpu-current-thread";
const char* LinuxSchedStateBlock::kIrqCpuNotification = "irq-cpu";

namespace
{

enum NotificationTypes
{
    kThreadStatusNotificationIdx = 0,
    kThreadPpidNotificationIdx,
    kThreadExecNotificationIdx,
    kThreadSyscallNotificationIdx,
    kCpuStatusNotificationIdx,
    kCpuCurrentThreadNotificationIdx,
    kIrqCpuNotificationIdx,
    kNumNotifications,
};

const char* kNotifications[] = {
    LinuxSchedStateBlock::kThreadStatusNotification,
    LinuxSchedStateBlock::kThreadPpidNotification,
    LinuxSchedStateBlock::kThreadExecNotification,
    LinuxSchedStateBlock::kThreadSyscallNotification,
    LinuxSchedStateBlock::kCpuStatusNotification,
    LinuxSchedStateBlock::kCpuCurrentThreadNotification,
    LinuxSchedStateBlock::kIrqCpuNotification,
};

}  // namespace

LinuxSchedStateBlock::LinuxSchedStateBlock()
{
}

void LinuxSchedStateBlock::GetNotificationSinks(notification::NotificationCenter* notificationCenter)
{
    notification::KeyPath keyPath {Token("state"), Token("linux-sched"), Token("")};
    for (size_t i = 0; i < kNumNotifications; ++i)
    {
        keyPath.back() = Token(kNotifications[i]);
        _sinks.push_back(notificationCenter->GetNotificationSink(keyPath));
    }
}

void LinuxSchedStateBlock::RegisterNotificationObservers(notification::NotificationCenter* notificationCenter)
{
    namespace pl = std::placeholders;

    KernelObserver(notificationCenter, Token("exit_syscall"), std::bind(&LinuxSchedStateBlock::onExitSyscall, this, pl::_1));
    KernelObserver(notificationCenter, Token("irq_handler_entry"), std::bind(&LinuxSchedStateBlock::onIrqHandlerEntry, this, pl::_1));
    KernelObserver(notificationCenter, Token("irq_handler_exit"), std::bind(&LinuxSchedStateBlock::onIrqHandlerExit, this, pl::_1));
    KernelObserver(notificationCenter, Token("softirq_entry"), std::bind(&LinuxSchedStateBlock::onSoftIrqEntry, this, pl::_1));
    KernelObserver(notificationCenter, Token("softirq_exit"), std::bind(&LinuxSchedStateBlock::onSoftIrqExit, this, pl::_1));
    KernelObserver(notificationCenter, Token("softirq_raise"), std::bind(&LinuxSchedStateBlock::onSoftIrqRaise, this, pl::_1));
    KernelObserver(notificationCenter, Token("sched_switch"), std::bind(&LinuxSchedStateBlock::onSchedSwitch, this, pl::_1));
    KernelObserver(notificationCenter, Token("sched_process_fork"), std::bind(&LinuxSchedStateBlock::onSchedProcessFork, this, pl::_1));
    KernelObserver(notificationCenter, Token("sched_process_free"), std::bind(&LinuxSchedStateBlock::onSchedProcessFree, this, pl::_1));
    KernelObserver(notificationCenter, Token("lttng_statedump_process_state"), std::bind(&LinuxSchedStateBlock::onLttngStatedumpProcessState, this, pl::_1));
    KernelObserver(notificationCenter, RegexToken("^sched_wakeup"), std::bind(&LinuxSchedStateBlock::onSchedWakeupEvent, this, pl::_1));
    KernelObserver(notificationCenter, RegexToken("^sys_"), std::bind(&LinuxSchedStateBlock::onSysEvent, this, pl::_1));
    KernelObserver(notificationCenter, RegexToken("^compat_sys_"), std::bind(&LinuxSchedStateBlock::onSysEvent, this, pl::_1));
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
    PostNotification<value::UIntValue>(
        kThreadStatusNotificationIdx, _currentState->GetAttributeKeyStr({"test"}), 42);
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
