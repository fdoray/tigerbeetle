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
#ifndef _TIBEE_TRACE_ARRAYEVENTVALUE_HPP
#define _TIBEE_TRACE_ARRAYEVENTVALUE_HPP

#include <cstddef>
#include <vector>
#include <babeltrace/babeltrace.h>
#include <babeltrace/ctf/events.h>

#include "value/Value.hpp"

namespace tibee
{
namespace trace
{

// Forward declaration.
class EventValueFactory;

/**
 * Event value carrying an array of values.
 *
 * @author Philippe Proulx
 */
class ArrayEventValue :
    public value::ArrayValueBase
{
public:
    /**
     * Builds an array value out of a field definition and an event.
     *
     * @param def          BT field definition
     * @param ev           BT event
     * @param valueFactory Value factory used to create other event values
     */
    ArrayEventValue(const ::bt_definition* def, const ::bt_ctf_event* ev,
                    const EventValueFactory* valueFactory);

    // Overridden from ArrayValueBase.
    virtual std::size_t Length() const override;
    virtual const value::Value* at(size_t index) const override;
    virtual value::ArrayValueBase::Iterator values_begin() const override;
    virtual value::ArrayValueBase::Iterator values_end() const override;

private:
    // Implementation of an array iterator.
    class IteratorImpl :
        public value::ArrayValueBase::IteratorImpl
    {
    public:
        IteratorImpl(const ArrayEventValue* arrayValue, size_t index);

        virtual value::ArrayValueBase::IteratorImpl& operator++() override;
        virtual bool operator==(
            const value::ArrayValueBase::IteratorImpl& other) const override;
        virtual bool operator!=(
            const value::ArrayValueBase::IteratorImpl& other) const override;
        virtual const value::Value& operator*() const override;
        virtual const value::Value* operator->() const override;

    private:
        const ArrayEventValue* _arrayValue;
        size_t _currentIndex;
    };

private:
    void buildCache();

    const ::bt_definition* _btDef;
    const ::bt_ctf_event* _btEvent;
    ::bt_definition const* const* _btFieldList;
    std::size_t _size;
    const EventValueFactory* _valueFactory;
};

}
}

#endif // _TIBEE_TRACE_ARRAYEVENTVALUE_HPP
