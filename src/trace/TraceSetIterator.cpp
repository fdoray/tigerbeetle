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
#include <babeltrace/ctf/iterator.h>

#include "trace/TraceSetIterator.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee
{
namespace trace
{

TraceSetIterator::TraceSetIterator(::bt_ctf_iter* btCtfIter) :
    _btCtfIter {btCtfIter},
    _btIter {nullptr}
{
    if (!_btCtfIter) {
        return;
    }

    _btIter = ::bt_ctf_get_iter(_btCtfIter);

    // read current event
    _btEvent = ::bt_ctf_iter_read_event(_btCtfIter);

    // end?
    if (!_btEvent) {
        _btIter = nullptr;
        _btCtfIter = nullptr;
        return;
    }

    // create event
    _event = std::unique_ptr<EventValue> {
        new EventValue {std::addressof(_valueFactory)}
    };

    // update event wrapper
    _event->setPrivateEvent(_btEvent);
}

TraceSetIterator::TraceSetIterator(const TraceSetIterator& it)
{
    // invoke assignment operator
    *this = it;
}

TraceSetIterator::~TraceSetIterator()
{
    // do not destroy BT iterator; not own by us
}

TraceSetIterator& TraceSetIterator::operator=(const TraceSetIterator& rhs)
{
    /* Simply copy the BT iterator here. The copied iterator will follow
     * the original iterator since trace set iterators do not own their
     * BT iterator.
     */
    _btIter = rhs._btIter;
    _btCtfIter = rhs._btCtfIter;
    _btEvent = rhs._btEvent;

    return *this;
}

TraceSetIterator& TraceSetIterator::operator++()
{
    if (!_btIter) {
        // disabled
        return *this;
    }

    if (::bt_iter_next(_btIter) < 0) {
        // disable this iterator
        _btIter = nullptr;
        _btCtfIter = nullptr;
        return *this;
    }

    // read current event
    _btEvent = ::bt_ctf_iter_read_event(_btCtfIter);

    // end?
    if (!_btEvent) {
        _btIter = nullptr;
        _btCtfIter = nullptr;
        return *this;
    }

    // reset value factory pools
    _valueFactory.resetPools();

    // update event wrapper
    _event->setPrivateEvent(_btEvent);

    return *this;
}

bool TraceSetIterator::operator==(const TraceSetIterator& rhs)
{
    return _btIter == rhs._btIter;
}

bool TraceSetIterator::operator!=(const TraceSetIterator& rhs)
{
    return !(*this == rhs);
}

const EventValue& TraceSetIterator::operator*() const
{
    /* Behaviour is undefined (could crash) when we're at the end (should
     * be checked first by comparing to and end trace set iterator).
     */

    return *_event;
}

}
}
