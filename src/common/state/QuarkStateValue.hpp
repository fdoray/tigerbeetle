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
#ifndef _TIBEE_COMMON_QUARKSTATEVALUE_HPP
#define _TIBEE_COMMON_QUARKSTATEVALUE_HPP

#include <memory>
#include <cstdint>

#include <common/quark/Quark.hpp>
#include <common/state/AbstractStateValue.hpp>
#include <common/state/StateValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * Quark state value (32-bit unsigned integer referencing
 * data elsewhere).
 *
 * @author Philippe Proulx
 */
class QuarkStateValue :
    public AbstractStateValue
{
public:
    typedef std::shared_ptr<QuarkStateValue> SP;
    typedef std::unique_ptr<QuarkStateValue> UP;

public:
    /**
     * Builds a quark state value.
     *
     * @param quark Quark value
     */
    QuarkStateValue(Quark quark) :
        AbstractStateValue {StateValueType::QUARK},
        _quark {quark}
    {
    }

    /**
     * Returns the actual quark value.
     *
     * @returns Quark value
     */
    Quark getValue() const
    {
        return _quark;
    }

private:
    Quark _quark;
};

}
}

#endif // _TIBEE_COMMON_QUARKSTATEVALUE_HPP
