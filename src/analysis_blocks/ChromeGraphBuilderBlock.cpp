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
    
    if (execName != "chrome")
        return;

    uint32_t tid = atoi(path[kTidPathIndex].token().c_str());
    auto ppid = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_PPID})->AsInteger();
    if (_analyzedThreads.find(ppid) != _analyzedThreads.end())
        return;
    if (!_graphBuilder->AddGraph(tid, execName))
        return;

    quark::Quark qStatus = Q_WAIT_FOR_CPU;
    auto status = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_STATUS});
    if (status != nullptr)
        qStatus = quark::Quark(status->AsUInteger());

    _graphBuilder->StartTimer(tid, qStatus);

    // Check whether we are in a syscall.
    auto currentSycall = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_SYSCALL});
    if (currentSycall != nullptr)
    {
        _graphBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(currentSycall->AsString()));
    }

    _analyzedThreads.insert(tid);
}

void ChromeGraphBuilderBlock::onChromeTracing(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);
    char phase = event->getEventField(kPhaseField)->AsInteger();
    
    if (phase != kPhaseComplete &&
        phase != kPhaseBegin &&
        phase != kPhaseEnd &&
        phase != kPhaseFlowBegin &&
        phase != kPhaseFlowEnd)
    {
        return;
    }

    auto cpu = event->getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
    auto tid = currentThreadForCpu(cpu);
    if (tid == kInvalidThread) {
        std::cout << "invalid tid..." << std::endl;
        return;
    }

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
    /*
    // TODO: Move this to a common builder.

    uint32_t tid = atoi(path[kTidPathIndex].token().c_str());

    // Stop timer for previous status.
    auto previousStatus = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_STATUS});
    if (previousStatus != nullptr)
        _graphBuilder->StopTimer(tid, quark::Quark(previousStatus->AsUInteger()));

    // Start timer for next status.
    auto nextStatus = value->GetField(CurrentStateBlock::kAttributeValueField);
    if (nextStatus != nullptr)
        _graphBuilder->StartTimer(tid, quark::Quark(nextStatus->AsUInteger()));
    */
}

void ChromeGraphBuilderBlock::onSyscallChange(const notification::Path& path, const value::Value* value)
{
    /*
    // TODO: Move this to a common builder.

    uint32_t tid = atoi(path[kTidPathIndex].token().c_str());

    // Create a new node if the duration of the previous node is not 0.
    auto previousNodeTime = _graphBuilder->ReadTimer(tid, Q_DURATION);
    if (previousNodeTime != 0)
    {
        _graphBuilder->AddTaskStep(tid);
        _graphBuilder->SetProperty(tid, Q_NODE_TYPE, nullptr);
    }

    // Starting a sycall.
    auto nextSyscall = value->GetField(CurrentStateBlock::kAttributeValueField);
    if (nextSyscall != nullptr)
    {
        _graphBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(nextSyscall->AsString()));
    }
    */
}

void ChromeGraphBuilderBlock::onSchedProcessFork(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);

    auto parent_tid = event->getEventField("parent_tid")->AsUInteger();
    auto child_tid = event->getEventField("child_tid")->AsUInteger();

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

    if (tid == 1724)
    {
        theStack.push_back(name);
        std::cout << "stack " << _graphBuilder->GetTimestamp() << "                push " << name << std::endl;
        for (int i = 0; i < theStack.size(); ++i)
            std::cout << "    " << i << "\t" << theStack[i] << std::endl;
        std::cout <<std::endl;
    }
}

void ChromeGraphBuilderBlock::onPhaseEnd(uint32_t tid, const trace::EventValue& event)
{
    std::string name = event.getEventField(kNameField)->AsString();
    _graphBuilder->PopStack(tid);

    if (tid == 1724)
    {
        if (theStack.empty())
        {
            std::cout << "trying to pop " << name  << " but stack is empty" << std::endl;
            std::cout << std::endl;
        }
        else
        {
        if (theStack.back() != name)
        {
            std::cout << "pop problem..." << std::endl;
        }

        theStack.pop_back();
        std::cout << "stack " << _graphBuilder->GetTimestamp() << "                pop " << name << std::endl;
        for (int i = 0; i < theStack.size(); ++i)
            std::cout << "    " << i << "\t" << theStack[i] << std::endl;
        std::cout <<std::endl;
        }
    }
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

    //////////////////////////////////////////////////////////////////////////////////
    // Nothing should already be on this thread.... TODO

    if (tid == 1724)
    {

    if (_graphBuilder->HasNodeForThread(tid))
    {
        auto previousNameValue = _graphBuilder->GetProperty(tid, Q_NODE_TYPE);
        std::string previousName;
        if (previousNameValue != nullptr)
            previousName = previousNameValue->AsString();
        std::cout << _graphBuilder->GetTimestamp() << " Nothing should be scheduled on this thread... " << std::endl;
    }


    uint64_t task_id = event.getEventField(kIdField)->AsULong();
    _graphBuilder->ScheduleTask(task_id, tid);
    std::cout << _graphBuilder->GetTimestamp() << " schedule ";
    if (category == kCategoryTopLevelFlow)
        std::cout << "msg ";
    else
        std::cout<< "ipc ";
    std::cout << "on thread " << tid << std::endl;
    std::cout << std::endl;

    }
    //////////////////////////////////////////////////////////////////////////////////

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
