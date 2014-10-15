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
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "trace/value/EventValue.hpp"
#include "trace_blocks/TraceBlock.hpp"
#include "value/MakeValue.hpp"
#include "value/ReadValue.hpp"
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
    serviceList.QueryService(kGraphBuilderServiceName,
                             reinterpret_cast<void**>(&_graphBuilder));

    serviceList.QueryService(CurrentStateBlock::kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    // Get constant quarks.
    Q_LINUX = _currentState->Quark(kStateLinux);
    Q_THREADS = _currentState->Quark(kStateThreads);
    Q_SYSCALL = _currentState->Quark(kStateSyscall);
    Q_STATUS = _currentState->Quark(kStateStatus);
    Q_WAIT_FOR_CPU = _currentState->Quark(kStateWaitForCpu);
    Q_DURATION = _currentState->Quark(kDuration);
    Q_NODE_TYPE = _currentState->Quark(kNodeType);
}

void LinuxGraphBuilderBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateExecName)},
        base::BindObject(&LinuxGraphBuilderBlock::onExecName, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token("lttng-kernel"), Token("sched_process_fork")},
        base::BindObject(&LinuxGraphBuilderBlock::onSchedProcessFork, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token("lttng-kernel"), Token("sched_process_exit")},
        base::BindObject(&LinuxGraphBuilderBlock::onSchedProcessExit, this));
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateStatus)},
        base::BindObject(&LinuxGraphBuilderBlock::onStatusChange, this));
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateSyscall)},
        base::BindObject(&LinuxGraphBuilderBlock::onSyscallChange, this));
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
    if (!_graphBuilder->AddGraph(tid, execName))
        return;

    quark::Quark qStatus = Q_WAIT_FOR_CPU;
    auto status = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_STATUS});
    if (status != nullptr)
        qStatus = quark::Quark(status->AsUInteger());

    _graphBuilder->StartTimer(tid, qStatus);
    _graphBuilder->StartTimer(tid, Q_DURATION);

    // Check whether we are in a syscall.
    auto currentSycall = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_SYSCALL});
    if (currentSycall != nullptr)
    {
        _graphBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(currentSycall->AsString()));
    }
}

void LinuxGraphBuilderBlock::onSchedProcessFork(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);

    auto parent_tid = event->getEventField("parent_tid")->AsUInteger();
    auto child_tid = event->getEventField("child_tid")->AsUInteger();

    _graphBuilder->AddChildTask(parent_tid, child_tid);
    _graphBuilder->StartTimer(child_tid, Q_DURATION);
}

void LinuxGraphBuilderBlock::onSchedProcessExit(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);

    auto tid = event->getEventField("tid")->AsUInteger();
    _graphBuilder->EndTask(tid);
}

void LinuxGraphBuilderBlock::onStatusChange(const notification::Path& path, const value::Value* value)
{
    uint64_t tid = atoi(path[kTidPathIndex].token().c_str());

    // Stop timer for previous status.
    auto previousStatus = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_STATUS});
    if (previousStatus != nullptr)
        _graphBuilder->StopTimer(tid, quark::Quark(previousStatus->AsUInteger()));

    // Start timer for next status.
    auto nextStatus = value->GetField(CurrentStateBlock::kAttributeValueField);
    if (nextStatus != nullptr)
        _graphBuilder->StartTimer(tid, quark::Quark(nextStatus->AsUInteger()));
}

void LinuxGraphBuilderBlock::onSyscallChange(const notification::Path& path, const value::Value* value)
{
    uint64_t tid = atoi(path[kTidPathIndex].token().c_str());

    // Create a new node if the duration of the previous node is not 0.
    auto previousNodeTime = _graphBuilder->ReadTimer(tid, Q_DURATION);
    if (previousNodeTime != 0)
    {
        _graphBuilder->AddTaskStep(tid);
    }

    // Starting a sycall.
    auto nextSyscall = value->GetField(CurrentStateBlock::kAttributeValueField);
    if (nextSyscall != nullptr)
    {
        _graphBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(nextSyscall->AsString()));
    }
}

}  // namespace analysis_blocks
}  // namespace tibee
