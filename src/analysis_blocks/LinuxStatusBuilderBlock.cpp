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
#include "analysis_blocks/LinuxStatusBuilderBlock.hpp"

#include <stdlib.h>

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "trace/value/EventValue.hpp"
#include "value/MakeValue.hpp"
#include "value/ReadValue.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace analysis_blocks {

namespace
{
using notification::AnyToken;
using notification::Token;
using state_blocks::CurrentStateBlock;

const size_t kTidPathIndex = 3;
}  // namespace

LinuxStatusBuilderBlock::LinuxStatusBuilderBlock()
{
}

void LinuxStatusBuilderBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kGraphBuilderServiceName,
                             reinterpret_cast<void**>(&_graphBuilder));

    serviceList.QueryService(CurrentStateBlock::kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    Q_LINUX = _currentState->Quark(kStateLinux);
    Q_THREADS = _currentState->Quark(kStateThreads);
    Q_STATUS = _currentState->Quark(kStateStatus);
}

void LinuxStatusBuilderBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(CurrentStateBlock::kNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateStatus)},
        base::BindObject(&LinuxStatusBuilderBlock::onStatusChange, this));
}

void LinuxStatusBuilderBlock::onStatusChange(const notification::Path& path, const value::Value* value)
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

}  // namespace analysis_blocks
}  // namespace tibee
