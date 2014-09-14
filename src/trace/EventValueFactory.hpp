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
#ifndef _TIBEE_TRACE_EVENTVALUEFACTORY_HPP
#define _TIBEE_TRACE_EVENTVALUEFACTORY_HPP

#include <array>
#include <functional>
#include <babeltrace/ctf/events.h>

#include "trace/EventValuePool.hpp"
#include "trace/value/ArrayEventValue.hpp"
#include "trace/value/StructEventValue.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace trace
{

/**
 * Event value factory.
 *
 * This is the appropriate object to use for creating event values in an
 * efficient and portable way.
 *
 * @author Philippe Proulx
 */
class EventValueFactory
{
public:
    /**
     * Builds an event value factory.
     */
    EventValueFactory();

    /**
     * Destroys an event value factory.
     *
     * Any event value built using this factory will not be valid when
     * it's destroyed.
     */
    ~EventValueFactory();

    /**
     * Returns an abstract event value out of a BT definition/event pair,
     * potentially building it.
     *
     * Caller doesn't own this pointer and should not free it.
     *
     * @param def BT field definition
     * @param ev  BT event
     * @returns   Abstract event value for this event/definition pair
     */
    const value::Value* buildEventValue(const ::bt_definition* def,
                                        const ::bt_ctf_event* ev) const;

    /**
     * Resets all internal pools.
     */
    void resetPools();

private:
    typedef std::function<const value::Value* (const ::bt_definition*, const ::bt_ctf_event* ev)> BuildValueFunc;

private:
    void initTypes();

private:
    // array mapping (CTF types -> event value builder functions)
    std::array<BuildValueFunc, 32> _builders;

    // our object pools
    EventValuePool<ArrayEventValue> _arrayPool;
    EventValuePool<StructEventValue> _structPool;
    EventValuePool<value::ULongValue> _enumPool;
    EventValuePool<value::DoubleValue> _doublePool;
    EventValuePool<value::LongValue> _longPool;
    EventValuePool<value::StringValue> _stringPool;
    EventValuePool<value::ULongValue> _ulongPool;

    // TODO(fdoray): Add an enum type.
};

}
}

#endif // _TIBEE_TRACE_EVENTVALUEFACTORY_HPP
