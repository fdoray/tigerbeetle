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

#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
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
class LinuxSchedStateBlock : public block::AbstractBlock
{
public:
    LinuxSchedStateBlock();

    virtual void GetNotificationSinks(notification::NotificationCenter* notificationCenter) override;
    virtual void RegisterNotificationObservers(notification::NotificationCenter* notificationCenter) override;

    static const char* kThreadStatusNotification;
    static const char* kThreadPpidNotification;
    static const char* kThreadExecNotification;
    static const char* kThreadSyscallNotification;
    static const char* kCpuStatusNotification;
    static const char* kCpuCurrentThreadNotification;
    static const char* kIrqCpuNotification;

private:
    typedef void (LinuxSchedStateBlock::*EventHandler)(const trace::EventValue& event);
    void RegisterNotificationObserver(notification::NotificationCenter* notificationCenter,
                                      notification::Token token,
                                      EventHandler eventHandler);

    void onEvent(const value::Value* event, EventHandler handler);
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

    notification::NotificationSink* _sinks[kNumNotifications];
};

}
}

#endif // _TIBEE_TRACEBLOCKS_TRACEBLOCK_HPP
