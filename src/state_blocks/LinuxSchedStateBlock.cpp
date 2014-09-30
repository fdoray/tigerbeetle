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

#include "notification/NotificationCenter.hpp"

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
    for (size_t i = 0; i < kNumNotifications; ++i)
        _sinks[i] = nullptr;
}

void LinuxSchedStateBlock::GetNotificationSinks(notification::NotificationCenter* notificationCenter)
{
    notification::KeyPath keyPath {Token("linux-sched-state"), Token("")};
    for (size_t i = 0; i < sizeof(kNotifications) / sizeof(char*); ++i)
    {
        keyPath.back() = Token(kNotifications[i]);
        _sinks[i] = notificationCenter->GetNotificationSink(keyPath);
    }
}

void LinuxSchedStateBlock::RegisterNotificationObservers(notification::NotificationCenter* notificationCenter)
{
    RegisterNotificationObserver(notificationCenter, Token("exit_syscall"), &LinuxSchedStateBlock::onExitSyscall);
    RegisterNotificationObserver(notificationCenter, Token("irq_handler_entry"), &LinuxSchedStateBlock::onIrqHandlerEntry);
    RegisterNotificationObserver(notificationCenter, Token("irq_handler_exit"), &LinuxSchedStateBlock::onIrqHandlerExit);
    RegisterNotificationObserver(notificationCenter, Token("softirq_entry"), &LinuxSchedStateBlock::onSoftIrqEntry);
    RegisterNotificationObserver(notificationCenter, Token("softirq_exit"), &LinuxSchedStateBlock::onSoftIrqExit);
    RegisterNotificationObserver(notificationCenter, Token("softirq_raise"), &LinuxSchedStateBlock::onSoftIrqRaise);
    RegisterNotificationObserver(notificationCenter, Token("sched_switch"), &LinuxSchedStateBlock::onSchedSwitch);
    RegisterNotificationObserver(notificationCenter, Token("sched_process_fork"), &LinuxSchedStateBlock::onSchedProcessFork);
    RegisterNotificationObserver(notificationCenter, Token("sched_process_free"), &LinuxSchedStateBlock::onSchedProcessFree);
    RegisterNotificationObserver(notificationCenter, Token("lttng_statedump_process_state"), &LinuxSchedStateBlock::onLttngStatedumpProcessState);
    RegisterNotificationObserver(notificationCenter, RegexToken("^sched_wakeup"), &LinuxSchedStateBlock::onSchedWakeupEvent);
    RegisterNotificationObserver(notificationCenter, RegexToken("^sys_"), &LinuxSchedStateBlock::onSysEvent);
    RegisterNotificationObserver(notificationCenter, RegexToken("^compat_sys_"), &LinuxSchedStateBlock::onSysEvent);
}

void LinuxSchedStateBlock::RegisterNotificationObserver(notification::NotificationCenter* notificationCenter,
                                                        notification::Token token,
                                                        EventHandler eventHandler)
{
    namespace pl = std::placeholders;

    notification::KeyPath path {Token("event"), Token("lttng-kernel"), token};
    notification::NotificationCenter::OnNotificationFunc func = std::bind(&LinuxSchedStateBlock::onEvent,
                  this,
                  pl::_2,
                  eventHandler);
    notificationCenter->RegisterNotificationObserver(path, func);
}

void LinuxSchedStateBlock::onEvent(const value::Value* event, EventHandler handler)
{
    if (event == nullptr)
        return;
    (this->*handler)(*reinterpret_cast<const trace::EventValue*>(event));
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
