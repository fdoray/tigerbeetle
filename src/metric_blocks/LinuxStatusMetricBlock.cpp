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
#include "metric_blocks/LinuxStatusMetricBlock.hpp"

#include <stdlib.h>

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "trace/value/EventValue.hpp"
#include "value/MakeValue.hpp"
#include "value/ReadValue.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace metric_blocks {

namespace
{
using notification::AnyToken;
using notification::Token;

const size_t kTidPathIndex = 3;
}  // namespace

LinuxStatusMetricBlock::LinuxStatusMetricBlock()
{
}

void LinuxStatusMetricBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kExecutionBuilderServiceName,
                             reinterpret_cast<void**>(&_executionBuilder));

    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    Q_LINUX = _currentState->Quark(kStateLinux);
    Q_THREADS = _currentState->Quark(kStateThreads);
    Q_STATUS = _currentState->Quark(kStateStatus);
}

void LinuxStatusMetricBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kCurrentStateNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateStatus)},
        base::BindObject(&LinuxStatusMetricBlock::onStatusChange, this));
}

void LinuxStatusMetricBlock::onStatusChange(const notification::Path& path, const value::Value* value)
{
    uint64_t tid = atoi(path[kTidPathIndex].token().c_str());

    // Stop timer for previous status.
    auto previousStatus = _currentState->GetAttributeValue(
        {Q_LINUX, Q_THREADS, _currentState->IntQuark(tid), Q_STATUS});
    if (previousStatus != nullptr)
        _executionBuilder->StopTimer(tid, quark::Quark(previousStatus->AsUInteger()));

    // Start timer for next status.
    auto nextStatus = value->GetField(kCurrentStateAttributeValueField);
    if (nextStatus != nullptr)
        _executionBuilder->StartTimer(tid, quark::Quark(nextStatus->AsUInteger()));
}

}  // namespace metric_blocks
}  // namespace tibee
