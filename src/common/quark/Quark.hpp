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
#ifndef _TIBEE_COMMON_QUARK_HPP
#define _TIBEE_COMMON_QUARK_HPP

#include <cstdint>

#include <common/BasicTypes.hpp>

namespace tibee
{
namespace common
{

/**
 * Quark.
 *
 * This is a quark, i.e. an integer which is associated with something
 * else (usually a string). The actual quark integer is wrapped by this
 * class in order to differentiate between quarks and integers in
 * overloaded methods accepting both. Quark objects are immutable.
 *
 * A good compiler will optimize all of the following methods out of
 * existence. Having a Quark class also forces the user to acknowledge
 * the semantic meaning of a quark parameter.
 *
 * @author Philippe Proulx
 */
class Quark
{
public:
    /**
     * Builds an empty quark with value 0.
     */
    explicit Quark() :
        _quark {0}
    {
    }

    /**
     * Builds a quark.
     *
     * @param quark Quark value
     */
    explicit Quark(quark_t quark) :
        _quark {quark}
    {
    }

    /**
     * Returns the underlying quark integer.
     */
    quark_t get() const
    {
        return _quark;
    }

    bool operator<(const Quark& q)
    {
        return _quark < q._quark;
    }

    bool operator==(const Quark& q)
    {
        return _quark == q._quark;
    }

    bool operator!=(const Quark& q)
    {
        return _quark != q._quark;
    }

private:
    quark_t _quark;
};

}
}

#endif // _TIBEE_COMMON_QUARK_HPP
