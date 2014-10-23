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
#include "analysis_blocks/ChromeGraphBuilderBlock.hpp"

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

const char kChromeExecName[] = "chrome";

const char kPhaseField[] = "phase";
const char kNameField[] = "name";
const char kCategoryField[] = "category";
const char kIdField[] = "id";

const char kPhaseBegin = 'B';
const char kPhaseEnd = 'E';
const char kPhaseComplete = 'X';
const char kPhaseInstant = 'I';
const char kPhaseAsyncBegin = 'S';
const char kPhaseAsyncStepInto = 'T';
const char kPhaseAsyncStepPast = 'p';
const char kPhaseAsyncEnd = 'F';
const char kPhaseNestableAsyncBegin = 'b';
const char kPhaseNestableAsyncEnd = 'e';
const char kPhaseNestableAsyncnstant = 'n';
const char kPhaseFlowBegin = 's';
const char kPhaseFlowStep = 't';
const char kPhaseFlowEnd = 'f';
const char kPhaseMetadata = 'M';
const char kPhaseCounter = 'C';
const char kPhaseSample = 'P';
const char kPhaseCreateObject = 'N';
const char kPhaseSnapshotObject = 'O';
const char kPhaseDeleteObject = 'D';

const char kCategoryTopLevelFlow[] = "disabled-by-default-toplevel.flow";
const char kCategoryIpcFlow[] = "disabled-by-default-ipc.flow";
const char kNameScheduler[] = "Scheduler";
const char kNameDispatchInputData[] = "ChannelReader::DispatchInputData";

const uint32_t kInvalidThread = -1;
}  // namespace

using base::tbendl;
using base::tbwarn;
using notification::AnyToken;
using notification::RegexToken;
using notification::Token;
using state_blocks::CurrentStateBlock;
using trace_blocks::TraceBlock;

ChromeGraphBuilderBlock::ChromeGraphBuilderBlock()
{
}

void ChromeGraphBuilderBlock::LoadServices(const block::ServiceList& serviceList)
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
    Q_CUR_THREAD = _currentState->Quark(kStateCurThread);
    Q_PPID = _currentState->Quark(kStatePpid);
    Q_DURATION = _currentState->Quark(kDuration);
    Q_NODE_TYPE = _currentState->Quark(kNodeType);

    // CPUs attribute.
    _cpusAttribute = _currentState->GetAttributeKey({
        Q_LINUX, _currentState->Quark(kStateCpus)});
}

void ChromeGraphBuilderBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateExecName)},
        base::BindObject(&ChromeGraphBuilderBlock::onExecName, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token("lttng-ust"), Token("chrome:tracing")},
        base::BindObject(&ChromeGraphBuilderBlock::onChromeTracing, this));
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateStatus)},
        base::BindObject(&ChromeGraphBuilderBlock::onStatusChange, this));
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateSyscall)},
        base::BindObject(&ChromeGraphBuilderBlock::onSyscallChange, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token("lttng-kernel"), Token("sched_process_fork")},
        base::BindObject(&ChromeGraphBuilderBlock::onSchedProcessFork, this));
}

void ChromeGraphBuilderBlock::onExecName(const notification::Path& path, const value::Value* value)
{
    auto attributeValue = value->GetField(CurrentStateBlock::kAttributeValueField);
    if (attributeValue == nullptr)
        return;
    std::string execName = attributeValue->AsString();
    
    if (execName != kChromeExecName)
        return;

    uint32_t tid = atoi(path[kTidPathIndex].token().c_str());

    auto ppidValue = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_PPID});

    if (ppidValue != nullptr &&
        _analyzedThreads.find(ppidValue->AsInteger()) != _analyzedThreads.end())
    {
        return;
    }

    if (!_graphBuilder->AddGraph(tid, execName))
    {
        return;
    }

    _analyzedThreads.insert(tid);
}

void ChromeGraphBuilderBlock::onChromeTracing(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);

    auto cpu = event->getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
    auto tid = currentThreadForCpu(cpu);

    if (tid == kInvalidThread) {
        tbwarn() << "No tid found for event on CPU " << cpu << "." << tbendl();
        return;
    }

    // Sanity check.
    auto ts = event->getEventField("ts")->AsULong();
    if (_counters.find(tid) == _counters.end())
    {
        _counters[tid] = ts;
    }
    else
    {
        if (ts != (_counters[tid] + 1))
        {
            std::cout << "counter gap: expected " << (_counters[tid] + 1) << " but got " << ts << std::endl;
        }
        _counters[tid] = ts;
    }

    // Handle events.
    char phase = event->getEventField(kPhaseField)->AsInteger();

    if (phase == kPhaseComplete || phase == kPhaseBegin)
        onPhaseBegin(tid, *event);
    else if (phase == kPhaseEnd)
        onPhaseEnd(tid, *event);
    else if (phase == kPhaseFlowBegin)
        onPhaseFlowBegin(tid, *event);
    else if (phase == kPhaseFlowEnd)
        onPhaseFlowEnd(tid, *event);
}

void ChromeGraphBuilderBlock::onStatusChange(const notification::Path& path, const value::Value* value)
{
}

void ChromeGraphBuilderBlock::onSyscallChange(const notification::Path& path, const value::Value* value)
{
}

void ChromeGraphBuilderBlock::onSchedProcessFork(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);

    auto parent_tid = event->getEventField("parent_tid")->AsUInteger();
    auto child_tid = event->getEventField("child_tid")->AsUInteger();

    // Keep track of the tids that belong to Chrome.
    if (_analyzedThreads.find(parent_tid) != _analyzedThreads.end())
        _analyzedThreads.insert(child_tid);
}

void ChromeGraphBuilderBlock::onPhaseBegin(uint32_t tid, const trace::EventValue& event)
{
    std::string name = event.getEventField(kNameField)->AsString();
    auto previousNameValue = _graphBuilder->GetProperty(tid, Q_NODE_TYPE);
    std::string previousName;
    if (previousNameValue != nullptr)
        previousName = previousNameValue->AsString();

    if (previousName == kNameScheduler)
        _graphBuilder->AddStepForThreadId(tid);
    else
        _graphBuilder->PushStack(tid);

    _graphBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(name));
}

void ChromeGraphBuilderBlock::onPhaseEnd(uint32_t tid, const trace::EventValue& event)
{
    std::string name = event.getEventField(kNameField)->AsString();
    _graphBuilder->PopStack(tid);
}

void ChromeGraphBuilderBlock::onPhaseFlowBegin(uint32_t tid, const trace::EventValue& event)
{
    std::string category = event.getEventField(kCategoryField)->AsString();
    if (category != kCategoryTopLevelFlow && category != kCategoryIpcFlow) 
        return;

    uint64_t task_id = event.getEventField(kIdField)->AsULong();
    _graphBuilder->AddChildTask(tid, task_id);
}

void ChromeGraphBuilderBlock::onPhaseFlowEnd(uint32_t tid, const trace::EventValue& event)
{
    std::string category = event.getEventField(kCategoryField)->AsString();
    if (category != kCategoryTopLevelFlow && category != kCategoryIpcFlow) 
        return;

    uint64_t task_id = event.getEventField(kIdField)->AsULong();
    _graphBuilder->ScheduleTask(task_id, tid);

    if (category == kCategoryTopLevelFlow)
        _graphBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(kNameScheduler));
    else
        _graphBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(kNameDispatchInputData));
}

uint32_t ChromeGraphBuilderBlock::currentThreadForCpu(uint32_t cpu)
{
    auto threadValue = _currentState->GetAttributeValue(
        _cpusAttribute,
        {_currentState->IntQuark(cpu), Q_CUR_THREAD});
    if (threadValue == nullptr)
        return kInvalidThread;
    return threadValue->AsInteger();
}

}  // namespace analysis_blocks
}  // namespace tibee
