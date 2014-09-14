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
#ifndef _TIBEE_TRACE_EVENTVALUEPOOL_HPP
#define _TIBEE_TRACE_EVENTVALUEPOOL_HPP

#include <cstddef>
#include <list>
#include <type_traits>

namespace tibee
{
namespace trace
{

/**
 * Event value pool template class.
 *
 * This is a specific object pool implementation for our use case: we
 * never want to "free" objects, only allocate them one after the other,
 * and free all the pool memory on destruction or "reset" it on demand
 * (not freeing anything, but effectively restarting allocation from
 * index 0 and resetting the size).
 *
 * The amount of system memory requested is determined using a doubling
 * algorithm; that is, each time more system memory is allocated, the
 * amount of system memory requested is doubled.
 *
 * The returned memory address when getting a new event value space is
 * guaranteed to respect the specified object type alignment.
 *
 * No constructor/destructor are called by this pool.
 *
 * @author Philippe Proulx
 */
template<typename T>
class EventValuePool
{
public:
    /**
     * Builds an event value pool.
     *
     * @param initCapacity Initial capacity of the pool in number of objects
     */
    EventValuePool(std::size_t initCapacity = 1);

    /**
     * Returns a free object from the pool. The object is not
     * constructed and never destroyed. It should not be freed by the
     * caller.
     */
    T* get();

    /**
     * Resets the pool (capacity stays as is, but the size goes back
     * to 0).
     */
    void reset();

private:
    typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type alignedT;

private:
    /* Actual pool. The pool is implemented with a linked list to avoid
     * the automatic reallocation of std::vector and such (which would free
     * the space actually used by event values somewhere in the wild).
     *
     * An iterator (see _nextIt below) always points to the _next_ pool
     * element, so the pool capacity (this list's size) must always be at
     * least 1. Calling _pool.resize() doesn't affect previous elements,
     * nor any current iterator.
     */
    std::list<alignedT> _pool;

    /* Current pool size (starts at 1 because the next element is always
     * preallocated).
     */
    std::size_t _size;

    // next pool element
    typename std::list<alignedT>::iterator _nextIt;
};

template<typename T>
EventValuePool<T>::EventValuePool(std::size_t initCapacity)
{
    if (initCapacity == 0) {
        initCapacity = 1;
    }

    _pool.resize(initCapacity);

    this->reset();
}

template<typename T>
T* EventValuePool<T>::get()
{
    auto ret = static_cast<T*>(static_cast<void*>(std::addressof(*_nextIt)));

    _size++;

    if (_size > _pool.size()) {
        _pool.resize(_pool.size() * 2);
    }

    _nextIt++;

    return ret;
}

template<typename T>
void EventValuePool<T>::reset()
{
    _size = 1;
    _nextIt = _pool.begin();
}

}
}

#endif // _TIBEE_TRACE_EVENTVALUEPOOL_HPP