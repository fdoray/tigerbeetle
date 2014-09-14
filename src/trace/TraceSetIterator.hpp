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
#ifndef _TIBEE_TRACE_TRACESETITERATOR_HPP
#define _TIBEE_TRACE_TRACESETITERATOR_HPP

#include <iterator>
#include <babeltrace/ctf/events.h>
#include <babeltrace/ctf/iterator.h>

#include "trace/EventValueFactory.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee
{
namespace trace
{

/**
 * A trace set iterator; returns a Value.
 *
 * Do not use this class directly; use an iterator returned by
 * TraceSet methods.
 *
 * Because of a limitation in libbabeltrace, i.e. two BT iterators
 * cannot exist concurrently in a single BT context, a trace set
 * iterator doesn't own its BT iterator. This means:
 *
 *   * the internal BT iterator won't be destroyed in the trace
 *     set iterator's destructor
 *   * copying a trace set iterator is okay, but all iterators obtained
 *     from a given trace set will always be synchronized (moved
 *     together)
 *
 * @author Philippe Proulx
 */
class TraceSetIterator :
    public std::iterator<std::input_iterator_tag, value::Value>
{
public:
    TraceSetIterator(::bt_ctf_iter* btCtfIter);
    TraceSetIterator(const TraceSetIterator& it);

    virtual ~TraceSetIterator();

    TraceSetIterator& operator=(const TraceSetIterator& rhs);
    TraceSetIterator& operator++();
    bool operator==(const TraceSetIterator& rhs);
    bool operator!=(const TraceSetIterator& rhs);

    /**
     * Returns the event currently pointed to by this iterator.
     *
     * The event stays valid as long as this iterator and any other
     * iterator for the same trace set remain untouched. Do NOT use
     * an event object after having called operator++().
     *
     * @returns Current event
     */
    const EventValue& operator*() const;

private:
    // libbabeltrace CTF iterator
    ::bt_ctf_iter* _btCtfIter;

    // libbabeltrace iterator
    ::bt_iter* _btIter;

    // current libbabeltrace event
    ::bt_ctf_event* _btEvent;

    // our only event root value object (constantly updated, not reallocated)
    // TODO: use shared_ptr here because the iterator may be copied
    std::unique_ptr<EventValue> _event;

    // the value factory used by this iterator and its event
    EventValueFactory _valueFactory;
};

}
}

#endif // _TIBEE_TRACE_TRACESETITERATOR_HPP
