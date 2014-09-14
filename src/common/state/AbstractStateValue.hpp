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
#ifndef _TIBEE_COMMON_ABSTRACTSTATEVALUE_HPP
#define _TIBEE_COMMON_ABSTRACTSTATEVALUE_HPP

#include <string>
#include <cstdint>
#include <memory>

#include <common/BasicTypes.hpp>
#include <common/quark/Quark.hpp>
#include <common/state/StateValueType.hpp>

namespace tibee
{
namespace common
{

class Sint32StateValue;
class Sint64StateValue;
class Uint32StateValue;
class Uint64StateValue;
class Float32StateValue;
class QuarkStateValue;
class NullStateValue;

/**
 * Abstract state value. All concrete state values must inherit
 * this class.
 *
 * @author Philippe Proulx
 */
class AbstractStateValue
{
public:
    /// Shared pointer to abstract state value
    typedef std::shared_ptr<AbstractStateValue> SP;

    /// Unique pointer to abstract state value
    typedef std::unique_ptr<AbstractStateValue> UP;

public:
    /**
     * Builds an abstract state value of a given type.
     *
     * @param type Type of state value
     */
    AbstractStateValue(StateValueType type);

    virtual ~AbstractStateValue() = 0;

    /**
     * Returns this state value's type.
     *
     * @returns State value type
     */
    StateValueType getType() const
    {
        return _type;
    }

    /**
     * Statically casts this state value to a 32-bit signed integer
     * state value.
     *
     * No runtime check is performed.
     *
     * @returns This state value as a 32-bit signed integer state value
     */
    const Sint32StateValue& asSint32Value() const;

    /**
     * Applies asSint32Value() and returns the actual integer value of
     * this object.
     *
     * No runtime check is performed, so if the actual state value type
     * is not Sint32StateValue, the behaviour is undefined.
     *
     * @returns This state value as a signed integer
     */
    std::int32_t asSint32() const;

    /**
     * Statically casts this state value to a 32-bit unsigned integer
     * state value.
     *
     * No runtime check is performed.
     *
     * @returns This state value as a 32-bit unsigned integer state value
     */
    const Uint32StateValue& asUint32Value() const;

    /**
     * Applies asUint32Value() and returns the actual integer value of
     * this object.
     *
     * No runtime check is performed, so if the actual state value type
     * is not Uint32StateValue, the behaviour is undefined.
     *
     * @returns This state value as an unsigned integer
     */
    std::uint32_t asUint32() const;

    /**
     * Statically casts this state value to a 64-bit signed integer
     * state value.
     *
     * No runtime check is performed.
     *
     * @returns This state value as a 64-bit signed integer state value
     */
    const Sint64StateValue& asSint64Value() const;

    /**
     * Applies asSint64Value() and returns the actual integer value of
     * this object.
     *
     * No runtime check is performed, so if the actual state value type
     * is not Sint64StateValue, the behaviour is undefined.
     *
     * @returns This state value as a signed integer
     */
    std::int64_t asSint64() const;

    /**
     * Statically casts this state value to a 64-bit unsigned integer
     * state value.
     *
     * No runtime check is performed.
     *
     * @returns This state value as a 64-bit unsigned integer state value
     */
    const Uint64StateValue& asUint64Value() const;

    /**
     * Applies asUint64Value() and returns the actual integer value of
     * this object.
     *
     * No runtime check is performed, so if the actual state value type
     * is not Uint64StateValue, the behaviour is undefined.
     *
     * @returns This state value as an unsigned integer
     */
    std::uint64_t asUint64() const;

    /**
     * Statically casts this state value to a 32-bit floating point
     * number state value.
     *
     * No runtime check is performed.
     *
     * @returns This state value as a 32-bit floating point number state value
     */
    const Float32StateValue& asFloat32Value() const;

    /**
     * Applies asFloat32Value() and returns the actual floating point
     * number value of this object.
     *
     * No runtime check is performed, so if the actual state value type
     * is not Float32StateValue, the behaviour is undefined.
     *
     * @returns This state value as a floating point number
     */
    float asFloat32() const;

    /**
     * Statically casts this state value to a quark state value.
     *
     * No runtime check is performed.
     *
     * @returns This state value as a quark state value
     */
    const QuarkStateValue& asQuarkValue() const;

    /**
     * Applies asQuarkValue() and returns the actual quark value
     * of this object.
     *
     * No runtime check is performed, so if the actual state value type
     * is not QuarkStateValue, the behaviour is undefined.
     *
     * @returns This state value as a quark
     */
    Quark asQuark() const;

    /**
     * Returns whether or not this is a 32-bit signed integer
     * state value.
     *
     * @returns True if this is a 32-bit signed integer state value
     */
    bool isSint32() const
    {
        return _type == StateValueType::SINT32;
    }

    /**
     * Returns whether or not this is a 64-bit signed integer
     * state value.
     *
     * @returns True if this is a 64-bit signed integer state value
     */
    bool isSint64() const
    {
        return _type == StateValueType::SINT64;
    }

    /**
     * Returns whether or not this is a 32-bit unsigned integer
     * state value.
     *
     * @returns True if this is a 32-bit unsigned integer state value
     */
    bool isUint32() const
    {
        return _type == StateValueType::UINT32;
    }

    /**
     * Returns whether or not this is a 64-bit signed integer
     * state value.
     *
     * @returns True if this is a 64-bit unsigned integer state value
     */
    bool isUint64() const
    {
        return _type == StateValueType::UINT64;
    }

    /**
     * Returns whether or not this is a 32-bit floating point number
     * state value.
     *
     * @returns True if this is a 32-bit floating point number state value
     */
    bool isFloat32() const
    {
        return _type == StateValueType::FLOAT32;
    }

    /**
     * Returns whether or not this is a quark state value.
     *
     * @returns True if this is a quark state value
     */
    bool isQuark() const
    {
        return _type == StateValueType::QUARK;
    }

    /**
     * Returns whether or not this state value is null.
     *
     * @returns True if this is a null state value
     */
    bool isNull() const
    {
        return _type == StateValueType::NUL;
    }

    /**
     * Not isNull().
     *
     * Returns true if this state value is not null.
     *
     * @see isNull()
     *
     * @returns True if this state value is not null
     */
    explicit operator bool() const
    {
        return !this->isNull();
    }

private:
    StateValueType _type;
};

}
}

#endif // _TIBEE_COMMON_ABSTRACTSTATEVALUE_HPP
