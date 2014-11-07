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
#include "execution_blocks/ExecutionBuilderBlock.hpp"

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Token.hpp"
#include "trace_blocks/TraceBlock.hpp"

namespace tibee {
namespace execution_blocks {

using notification::Token;
using trace_blocks::TraceBlock;

ExecutionBuilderBlock::ExecutionBuilderBlock()
{
}

ExecutionBuilderBlock::~ExecutionBuilderBlock()
{
}

void ExecutionBuilderBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kExecutionBuilderServiceName, &_executionBuilder);
}

void ExecutionBuilderBlock::LoadServices(const block::ServiceList& serviceList)
{
    quark::StringQuarkDatabase* quarks = nullptr;
    serviceList.QueryService(kQuarksServiceName,
                             reinterpret_cast<void**>(&quarks));
    _executionBuilder.SetQuarks(quarks);
}

void ExecutionBuilderBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kTimestampNotificationName)},
        base::BindObject(&ExecutionBuilderBlock::onTimestamp, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kEndNotificationName)},
        base::BindObject(&ExecutionBuilderBlock::onEnd, this));
}

void ExecutionBuilderBlock::GetNotificationSinks(notification::NotificationCenter* notificationCenter)
{
    _executionSink = notificationCenter->GetSink(
        {Token(kExecutionBuilderNotificationPrefix), Token(kExecutionBuilderNotificationName)});
}

void ExecutionBuilderBlock::onTimestamp(const notification::Path& path, const value::Value* value)
{
    _executionBuilder.SetTimestamp(value->AsULong());
}

void ExecutionBuilderBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    _executionBuilder.StopAllTimers();

    for (const auto& execution : _executionBuilder)
    {
        _executionSink->PostNotification(reinterpret_cast<const value::Value*>(execution.get()));
    }
}


}  // namespace execution_blocks
}  // namespace tibee
