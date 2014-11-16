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
#include "block/AbstractBlock.hpp"

#include "base/Constants.hpp"
#include "notification/Callback.hpp"
#include "notification/NotificationCenter.hpp"

namespace tibee
{
namespace block
{

namespace
{

using notification::AnyToken;
using notification::Token;

const size_t kTidPathIndex = 3;

void OnEvent(
    const value::Value* event,
    AbstractBlock::EventHandler handler)
{
    if (event == nullptr)
        return;
    handler(*reinterpret_cast<const trace::EventValue*>(event));
}

void OnThreadState(
    const notification::Path& path,
    const value::Value* value,
    AbstractBlock::ThreadStateHandler handler)
{
    uint32_t tid = atoi(path[kTidPathIndex].token().c_str());
    handler(tid, path, value);
}

}  // namespace

AbstractBlock::AbstractBlock()
{
}

void AbstractBlock::Start(const value::Value* parameters)
{
}

void AbstractBlock::GetNotificationSinks(notification::NotificationCenter* notificationCenter)
{
}

void AbstractBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
}

void AbstractBlock::RegisterServices(ServiceList* serviceList)
{
}

void AbstractBlock::LoadServices(const ServiceList& serviceList)
{
}

void AbstractBlock::Execute()
{
}

void AbstractBlock::Stop()
{
}


void AbstractBlock::AddKernelObserver(
    notification::NotificationCenter* notificationCenter,
    const notification::Token& token,
    EventHandler eventHandler)
{
    namespace pl = std::placeholders;

    notification::Path path {Token(kTraceNotificationPrefix),
                             Token("lttng-kernel"),
                             token};
    notification::Callback func =
        std::bind(&OnEvent,
                  pl::_2,
                  eventHandler);
    notificationCenter->AddObserver(path, func);
}

void AbstractBlock::AddUstObserver(
    notification::NotificationCenter* notificationCenter,
    const notification::Token& token,
    EventHandler eventHandler)
{
    namespace pl = std::placeholders;

    notification::Path path {Token(kTraceNotificationPrefix),
                             Token("lttng-ust"),
                             token};
    notification::Callback func =
        std::bind(&OnEvent,
                  pl::_2,
                  eventHandler);
    notificationCenter->AddObserver(path, func);
}

void AbstractBlock::AddThreadStateObserver(
    notification::NotificationCenter* notificationCenter,
    const notification::Token& token,
    ThreadStateHandler threadStateHandler)
{
    namespace pl = std::placeholders;

    notification::Path path {Token(kCurrentStateNotificationPrefix),
                             Token(kStateLinux),
                             Token(kStateThreads),
                             AnyToken(),
                             token};
    notification::Callback func =
        std::bind(&OnThreadState,
                  pl::_1,
                  pl::_2,
                  threadStateHandler);
    notificationCenter->AddObserver(path, func);
}


}  // namespace block
}  // namespace tibee
