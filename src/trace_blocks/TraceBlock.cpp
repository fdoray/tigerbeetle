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

using notification::Token;
using tibee::base::tbendl;
using tibee::base::tbwarn;

const char TraceBlock::kNotificationPrefix[] = "event";
const char TraceBlock::kTimestampNotificationName[] = "ts";

void TraceBlock::Start(const value::Value* params)
{
    _traceSet.reset(new trace::TraceSet);

    const value::ArrayValueBase* traceList = nullptr;
    if (!params->GetFieldAs("traces", &traceList))
        return;

    for (const auto& trace : *traceList)
    {
        std::string traceStr = trace.AsString();
        if (!_traceSet->addTrace(traceStr))
        {
            tbwarn() << "Trace " << traceStr << " could not be loaded." << tbendl();
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
            notification::Path keyPath = {
                Token(kNotificationPrefix),
                Token(traceInfos->getTraceType()),
                Token(eventNameIdPair.first)
            };

            auto traceId = traceInfos->getId();
            auto eventId = eventNameIdPair.second->getId();

            _eventSinks[traceId][eventId] = notificationCenter->GetSink(keyPath);
        }
    }

    _tsSink = notificationCenter->GetSink({
        Token(kNotificationPrefix),
        Token(kTimestampNotificationName)
    });
}

void TraceBlock::Execute()
{
    for (const auto& event : *_traceSet)
    {
        // Timestamp notification.
        _tsNotification.SetValue(event.getTimestamp());
        _tsSink->PostNotification(&_tsNotification);

        // Event notification.
        auto traceId = event.getTraceId();
        auto eventId = event.getId();

        auto traceIt = _eventSinks.find(traceId);
        assert(traceIt != _eventSinks.end());

        auto eventIt = traceIt->second.find(eventId);
        assert(eventIt != traceIt->second.end());

        eventIt->second->PostNotification(&event);
    }
}

}
}
