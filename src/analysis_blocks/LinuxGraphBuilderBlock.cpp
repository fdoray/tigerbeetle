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
#include "analysis_blocks/LinuxGraphBuilderBlock.hpp"

#include <stdlib.h>

#include "analysis_blocks/GraphBuilderBlock.hpp"
#include "base/BindObject.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "trace/value/EventValue.hpp"
#include "trace_blocks/TraceBlock.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace analysis_blocks {

namespace {
const size_t kTidPathIndex = 3;
}  // namespace

using notification::AnyToken;
using notification::RegexToken;
using notification::Token;
using state_blocks::CurrentStateBlock;
using trace_blocks::TraceBlock;

LinuxGraphBuilderBlock::LinuxGraphBuilderBlock()
{
}

void LinuxGraphBuilderBlock::Start(const value::Value* parameters)
{
    if (!value::ArrayValueBase::InstanceOf(parameters)) {
        base::tberror() << "LinuxGraphBuilderBlock received invalid parameters." << base::tbendl();
        return;
    }

    auto arrayParameters = value::ArrayValueBase::Cast(parameters);

    for (const auto& execName : *arrayParameters)
        _analyzedExecutables.insert(execName.AsString());
}

void LinuxGraphBuilderBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(CurrentStateBlock::kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    // Get constant quarks.
    Q_LINUX = _currentState->Quark("linux");
    Q_THREADS = _currentState->Quark("threads");
    Q_SYSCALL = _currentState->Quark("syscall");
    Q_STATUS = _currentState->Quark("status");
    Q_WAIT_FOR_CPU = _currentState->Quark("wait-for-cpu");
    Q_DURATION = _currentState->Quark("duration");
}

void LinuxGraphBuilderBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(TraceBlock::kNotificationPrefix), Token(TraceBlock::kTimestampNotificationName)},
        base::BindObject(&LinuxGraphBuilderBlock::onTimestamp, this));
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token("linux"), Token("threads"), AnyToken(), Token("exec-name")},
        base::BindObject(&LinuxGraphBuilderBlock::onExecName, this));
    notificationCenter->AddObserver(
        {Token(TraceBlock::kNotificationPrefix), Token("lttng-kernel"), Token("sched_process_fork")},
        base::BindObject(&LinuxGraphBuilderBlock::onSchedProcessFork, this));
    notificationCenter->AddObserver(
        {Token(TraceBlock::kNotificationPrefix), Token("lttng-kernel"), Token("sched_process_exit")},
        base::BindObject(&LinuxGraphBuilderBlock::onSchedProcessExit, this));
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token("linux"), Token("threads"), AnyToken(), Token("status")},
        base::BindObject(&LinuxGraphBuilderBlock::onStatusChange, this));
    notificationCenter->AddObserver(
        {Token(TraceBlock::kNotificationPrefix), Token(TraceBlock::kEndNotificationName)},
        base::BindObject(&LinuxGraphBuilderBlock::onEnd, this));
}

void LinuxGraphBuilderBlock::GetNotificationSinks(notification::NotificationCenter* notificationCenter)
{
    _graphSink = notificationCenter->GetSink({Token(kGraphBuilderNotificationPrefix), Token(kGraphNotificationName)});
}

void LinuxGraphBuilderBlock::onTimestamp(const notification::Path& path, const value::Value* value)
{
    _graphBuilder.SetTimestamp(value->AsULong());
}

void LinuxGraphBuilderBlock::onExecName(const notification::Path& path, const value::Value* value)
{
    auto attributeValue = value->GetField(CurrentStateBlock::kAttributeValueField);
    if (attributeValue == nullptr)
        return;
    std::string execName = attributeValue->AsString();

    if (_analyzedExecutables.find(execName) == _analyzedExecutables.end())
        return;

    uint64_t tid = atoi(path[kTidPathIndex].token().c_str());
    if (!_graphBuilder.AddGraph(tid, execName))
        return;

    quark::Quark qStatus = Q_WAIT_FOR_CPU;
    auto status = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_STATUS});
    if (status != nullptr)
        qStatus = quark::Quark(status->AsUInteger());

    _graphBuilder.StartTimer(tid, qStatus);
    _graphBuilder.StartTimer(tid, Q_DURATION);
}

void LinuxGraphBuilderBlock::onSchedProcessFork(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);

    auto parent_tid = event->getEventField("parent_tid")->AsUInteger();
    auto child_tid = event->getEventField("child_tid")->AsUInteger();

    _graphBuilder.AddChildTask(parent_tid, child_tid);
    _graphBuilder.StartTimer(child_tid, Q_DURATION);
}

void LinuxGraphBuilderBlock::onSchedProcessExit(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);

    auto tid = event->getEventField("tid")->AsUInteger();
    _graphBuilder.EndTask(tid);
}

void LinuxGraphBuilderBlock::onStatusChange(const notification::Path& path, const value::Value* value)
{
    uint64_t tid = atoi(path[kTidPathIndex].token().c_str());

    // Stop timer for previous status.
    auto previousStatus = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_STATUS});
    if (previousStatus != nullptr)
        _graphBuilder.StopTimer(tid, quark::Quark(previousStatus->AsUInteger()));

    // Start timer for next status.
    auto nextStatus = value->GetField(CurrentStateBlock::kAttributeValueField);
    if (nextStatus != nullptr)
        _graphBuilder.StartTimer(tid, quark::Quark(nextStatus->AsUInteger()));
}

void LinuxGraphBuilderBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    for (const auto& graph : _graphBuilder)
    {
        _graphSink->PostNotification(reinterpret_cast<const value::Value*>(graph.get()));
    }
}

}  // namespace analysis_blocks
}  // namespace tibee
