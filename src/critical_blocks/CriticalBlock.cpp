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
#include "critical_blocks/CriticalBlock.hpp"

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "critical/CriticalTypes.hpp"
#include "notification/NotificationCenter.hpp"

namespace tibee {
namespace critical_blocks {

using base::tbendl;
using base::tberror;
using notification::Token;

CriticalBlock::CriticalBlock()
{
}

void CriticalBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kCriticalGraphServiceName, &_graph);
}

void CriticalBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    Q_RUN_USERMODE = _currentState->Quark(kStateRunUsermode);
    Q_RUN_SYSCALL = _currentState->Quark(kStateRunSyscall);
    Q_WAIT_BLOCKED = _currentState->Quark(kStateWaitBlocked);
    Q_INTERRUPTED = _currentState->Quark(kStateInterrupted);
    Q_WAIT_FOR_CPU = _currentState->Quark(kStateWaitForCpu);
    Q_UNKNOWN = _currentState->Quark(kStateUnknown);
}

void CriticalBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    AddKernelObserver(notificationCenter, Token("sched_ttwu"), base::BindObject(&CriticalBlock::OnTTWU, this));
    AddThreadStateObserver(notificationCenter, Token(kStateStatus), base::BindObject(&CriticalBlock::OnThreadStatus, this));
}

void CriticalBlock::OnTTWU(const trace::EventValue& event)
{
    uint32_t source_tid = ThreadForEvent(event);
    uint32_t target_tid = event.getEventField("tid")->AsUInteger();

    if (source_tid != kInvalidThread && target_tid != kInvalidThread)
    {
        OnTTWUBetweenThreads(source_tid, target_tid);
    }

    // TODO: Handle wake-up from interrupt.
}

void CriticalBlock::OnTTWUBetweenThreads(uint32_t source_tid, uint32_t target_tid)
{
    // Cut the source thread.
    auto prevNodeSource = _graph.GetLastNodeForThread(source_tid);
    if (prevNodeSource == nullptr)
    {
        tberror() << "There should be a node on the source thread of a TTWU ("
            << source_tid << " > " << target_tid << ")." << tbendl();
        return;
    }
    auto prevTypeSourceIt = _lastEdgeTypePerThread.find(source_tid);
    if (prevTypeSourceIt == _lastEdgeTypePerThread.end() ||
        prevTypeSourceIt->second != critical::CriticalEdgeType::kRun)
    {
        tberror() << "Unexpected edge type for source of TTWU ("
            << source_tid << " > " << target_tid << ")." << tbendl();
        return;
    }

    auto nextNodeSource = _graph.CreateNode(_currentState->timestamp(), source_tid);
    _graph.CreateHorizontalEdge(prevTypeSourceIt->second, prevNodeSource, nextNodeSource);

    // Cut the target thread.
    auto prevNodeTarget = _graph.GetLastNodeForThread(target_tid);
    if (prevNodeTarget == nullptr)
    {
        // It's possible that we haven't seen the target of the TTWU
        // in the state dump yet.
        return;
    }

    auto prevTypeTargetIt = _lastEdgeTypePerThread.find(target_tid);
    if (prevTypeTargetIt == _lastEdgeTypePerThread.end())
    {
        tberror() << "There should be a prev type for target of TTWU ("
            << source_tid << " > " << target_tid << ")." << tbendl();
        return;
    }

    auto nextNodeTarget = _graph.CreateNode(_currentState->timestamp(), target_tid);
    _graph.CreateHorizontalEdge(prevTypeTargetIt->second, prevNodeTarget, nextNodeTarget);

    // Set the type of the next edge on the target to running.
    prevTypeTargetIt->second = critical::CriticalEdgeType::kRun;

    // Create the wake-up edge.
    _graph.CreateVerticalEdge(nextNodeSource, nextNodeTarget);
}

void CriticalBlock::OnThreadStatus(
    uint32_t tid,
    const notification::Path& path,
    const value::Value* value)
{
    // Determine the new edge type.
    auto newStatusValue = value->GetField(kCurrentStateAttributeValueField);
    critical::CriticalEdgeType newEdgeType = critical::CriticalEdgeType::kWaitOtherThread; 

    if (newStatusValue != nullptr)
    {
        quark::Quark qNewStatus(newStatusValue->AsUInteger());

        if (qNewStatus == Q_RUN_USERMODE ||
            qNewStatus == Q_RUN_SYSCALL ||
            qNewStatus == Q_INTERRUPTED)
        {
            newEdgeType = critical::CriticalEdgeType::kRun;
        }
    }

    // Get the last edge type for the thread.
    auto lookLastType = _lastEdgeTypePerThread.find(tid);
    if (lookLastType != _lastEdgeTypePerThread.end() &&
        lookLastType->second == newEdgeType)
    {
        return;
    }

    // Create the new node and add a link to it from the prev node.
    auto prevNode = _graph.GetLastNodeForThread(tid);
    auto newNode = _graph.CreateNode(_currentState->timestamp(), tid);

    if (prevNode != nullptr)
        _graph.CreateHorizontalEdge(lookLastType->second, prevNode, newNode);

    // Keep track of the type of the next edge.
    _lastEdgeTypePerThread[tid] = newEdgeType;
}

uint32_t CriticalBlock::ThreadForEvent(const trace::EventValue& event) const
{
    auto cpu = event.getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
    auto thread = _currentState->CurrentThreadForCpu(cpu);
    return thread;
}

}  // namespace critical_blocks
}  // namespace tibee
