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
#include "trace_blocks/TraceBlock.hpp"

#include "base/print.hpp"
#include "notification/NotificationCenter.hpp"

namespace tibee
{
namespace trace_blocks
{

using tibee::base::tbendl;
using tibee::base::tbwarn;

void TraceBlock::Start(const value::Value* params)
{
    _traceSet.reset(new trace::TraceSet);

    const value::ArrayValueBase* traceList = nullptr;
    if (!params->GetFieldAs("traces", &traceList))
        return;

    for (const auto& trace : *traceList)
    {
        std::string traceStr;
        if (trace.GetAsString(&traceStr))
        {
            if (!_traceSet->addTrace(traceStr))
            {
                tbwarn() << "Trace " << traceStr << " could not be loaded." << tbendl();
            }
        }
    }
}

void TraceBlock::GetNotificationSinks(notification::NotificationCenter* notificationCenter)
{
    const auto& tracesInfos = _traceSet->getTracesInfos();

    for (const auto& traceInfos : tracesInfos)
    {
        for (const auto& eventNameIdPair : *traceInfos->getEventMap())
        {
            notification::NotificationCenter::KeyPath keyPath = {
                notification::Token("event"),
                notification::Token(traceInfos->getTraceType()),
                notification::Token(eventNameIdPair.first)
            };

            auto traceId = traceInfos->getId();
            auto eventId = eventNameIdPair.second->getId();

            _sinks[traceId][eventId] = notificationCenter->GetNotificationSink(keyPath);
        }
    }
}

void TraceBlock::Execute(const block::ServiceList& serviceList)
{
    for (const auto& event : *_traceSet)
    {
        auto traceId = event.getTraceId();
        auto eventId = event.getId();

        auto traceIt = _sinks.find(traceId);
        assert(traceIt != _sinks.end());

        auto eventIt = traceIt->second.find(eventId);
        assert(eventIt != traceIt->second.end());

        eventIt->second->PostNotification(&event);
    }
}

}
}
