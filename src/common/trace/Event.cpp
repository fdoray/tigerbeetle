/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#include <memory>
#include <string>
#include <babeltrace/ctf/events.h>

#include <common/trace/EventValueType.hpp>
#include <common/trace/DictEventValue.hpp>
#include <common/trace/Event.hpp>

namespace tibee
{
namespace common
{

Event::Event()
{
}

const char* Event::getName() const
{
    return ::bt_ctf_event_name(_btEvent);
}

std::string Event::getNameStr() const
{
    return std::string {this->getName()};
}

trace_cycles_t Event::getCycles() const
{
    return static_cast<trace_cycles_t>(::bt_ctf_get_cycles(_btEvent));
}

trace_ts_t Event::getTimestamp() const
{
    return static_cast<trace_ts_t>(::bt_ctf_get_timestamp(_btEvent));
}

DictEventValue::UP Event::getTopLevelScope(::bt_ctf_scope topLevelScope) const
{
    // get fields scope
    auto scopeDef = ::bt_ctf_get_top_level_scope(_btEvent, topLevelScope);

    // make sure it's a struct
    if (scopeDef && ::bt_ctf_field_type(::bt_ctf_get_decl_from_def(scopeDef)) == ::CTF_TYPE_STRUCT) {
        auto dictEventValue = new DictEventValue {scopeDef, _btEvent};

        return DictEventValue::UP {dictEventValue};
    }

    return nullptr;
}

DictEventValue::UP Event::getFields() const
{
    return this->getTopLevelScope(::BT_EVENT_FIELDS);
}

DictEventValue::UP Event::getContext() const
{
    return this->getTopLevelScope(::BT_EVENT_CONTEXT);
}

DictEventValue::UP Event::getStreamEventContext() const
{
    return this->getTopLevelScope(::BT_STREAM_EVENT_CONTEXT);
}

DictEventValue::UP Event::getStreamPacketContext() const
{
    return this->getTopLevelScope(::BT_STREAM_PACKET_CONTEXT);
}

}
}