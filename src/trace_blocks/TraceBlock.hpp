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
#ifndef _TIBEE_TRACEBLOCKS_TRACEBLOCK_HPP
#define _TIBEE_TRACEBLOCKS_TRACEBLOCK_HPP

#include <unordered_map>

#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
#include "trace/BasicTypes.hpp"
#include "trace/TraceSet.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace trace_blocks
{

/**
 * A block that reads events from a trace.
 *
 * @author Francois Doray
 */
class TraceBlock : public block::AbstractBlock
{
public:
    static const char kNotificationPrefix[];
    static const char kTimestampNotificationName[];

    virtual void Start(const value::Value* params) override;
    virtual void GetNotificationSinks(notification::NotificationCenter* notificationCenter) override;
    virtual void Execute() override;

private:
    trace::TraceSet::UP _traceSet;

    // (event ID -> event callback) map
    typedef std::unordered_map<trace::event_id_t, const notification::NotificationSink*> EventIdSinkMap;

    // (trace ID -> (event ID -> event callback)) map
    typedef std::unordered_map<trace::trace_id_t, EventIdSinkMap> TraceIdEventIdSinkMap;

    // Sinks for events.
    TraceIdEventIdSinkMap _eventSinks;

    // Sink for timestamp.
    const notification::NotificationSink* _tsSink;

    // Timestamp notification.
    value::ULongValue _tsNotification;
};

}
}

#endif // _TIBEE_TRACEBLOCKS_TRACEBLOCK_HPP
