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
#ifndef _TIBEE_COMMON_STATENODE_HPP
#define _TIBEE_COMMON_STATENODE_HPP

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>

#include <common/BasicTypes.hpp>
#include <common/state/AbstractStateNodeVisitor.hpp>
#include <common/state/AbstractStateValue.hpp>
#include <common/state/Sint32StateValue.hpp>
#include <common/state/Sint64StateValue.hpp>
#include <common/state/Uint32StateValue.hpp>
#include <common/state/Uint64StateValue.hpp>
#include <common/state/Float32StateValue.hpp>
#include <common/state/QuarkStateValue.hpp>
#include <common/state/NullStateValue.hpp>
#include <common/state/StateNodeIterator.hpp>
#include <common/trace/AbstractEventValue.hpp>
#include <common/trace/StringEventValue.hpp>
#include <common/trace/SintEventValue.hpp>
#include <common/trace/UintEventValue.hpp>
#include <common/trace/FloatEventValue.hpp>

namespace tibee
{
namespace common
{

class CurrentState;

/**
 * A state node.
 *
 * A state node constitute the foundations of the state tree, of which
 * the root is held by a state history sink. A state node carries a
 * unique node ID, assigned by the state history sink, and zero to many
 * children, which are other state nodes.
 *
 * Each child node is identified by a quark; an integer associated to
 * a string by the state history sink. This associated string represents
 * the path part, or subpath of a child node relative to its parent. The
 * same path quark may be used to point multiple child nodes, although
 * they must not be siblings. Thus, a given node is always findable
 * using its complete path, which is a sequence of quarks. An empty
 * sequence corresponds to the root node.
 *
 * Each node holds a state value, which may be changed at any time, or
 * modified using convenience methods provided by the StateNode class.
 * Those changes eventually propagate to the state history sink, which
 * has the responsability of writing them as intervals on disk for later
 * queries.
 *
 * For children creation, StateNode behaves like std::map: using
 * operator[]() returns a reference to an existing child node, creating
 * it if it doesn't exist yet.
 *
 * @author Philippe Proulx
 */
#include <memory>

class StateNode :
    boost::noncopyable
{
    friend CurrentState;

    // friendship with iterator
    friend StateNodeIterator;

public:
    /// Unique pointer to StateNode
    typedef std::unique_ptr<StateNode> UP;

    /// State node iterator
    typedef StateNodeIterator Iterator;

public:
    ~StateNode();

    /**
     * Returns this node's ID.
     *
     * @returns Node ID
     */
    state_node_id_t getId() const
    {
        return _id;
    }

    /**
     * Returns the current begin timestamp of this node.
     *
     * @returns Current begin timestamp
     */
    timestamp_t getBeginTs() const
    {
        return _beginTs;
    }

    /**
     * Returns a const reference to the current state value of this
     * node.
     *
     * If no state value is set yet, returns a null state value const
     * reference.
     *
     * @returns Current state value or null state value const reference
     */
    const AbstractStateValue& getValue() const;

    /**
     * Forwarded to AbstractStateValue::asSint32Value() using this
     * node's state value.
     *
     * @see AbstractStateValue::asSint32Value()
     */
    const Sint32StateValue& asSint32Value() const
    {
        return _stateValue->asSint32Value();
    }

    /**
     * Forwarded to AbstractStateValue::asSint32() using this
     * node's state value.
     *
     * @see AbstractStateValue::asSint32()
     */
    std::int32_t asSint32() const
    {
        return _stateValue->asSint32();
    }

    /**
     * Forwarded to AbstractStateValue::asUint32Value() using this
     * node's state value.
     *
     * @see AbstractStateValue::asUint32Value()
     */
    const Uint32StateValue& asUint32Value() const
    {
        return _stateValue->asUint32Value();
    }

    /**
     * Forwarded to AbstractStateValue::asUint32() using this
     * node's state value.
     *
     * @see AbstractStateValue::asUint32()
     */
    std::uint32_t asUint32() const
    {
        return _stateValue->asUint32();
    }

    /**
     * Forwarded to AbstractStateValue::asSint64Value() using this
     * node's state value.
     *
     * @see AbstractStateValue::asSint64Value()
     */
    const Sint64StateValue& asSint64Value() const
    {
        return _stateValue->asSint64Value();
    }

    /**
     * Forwarded to AbstractStateValue::asSint64() using this
     * node's state value.
     *
     * @see AbstractStateValue::asSint64()
     */
    std::int64_t asSint64() const
    {
        return _stateValue->asSint64();
    }

    /**
     * Forwarded to AbstractStateValue::asUint64Value() using this
     * node's state value.
     *
     * @see AbstractStateValue::asUint64Value()
     */
    const Uint64StateValue& asUint64Value() const
    {
        return _stateValue->asUint64Value();
    }

    /**
     * Forwarded to AbstractStateValue::asUint64() using this
     * node's state value.
     *
     * @see AbstractStateValue::asUint64()
     */
    std::uint64_t asUint64() const
    {
        return _stateValue->asUint64();
    }

    /**
     * Forwarded to AbstractStateValue::asFloat32Value() using this
     * node's state value.
     *
     * @see AbstractStateValue::asFloat32Value()
     */
    const Float32StateValue& asFloat32Value() const
    {
        return _stateValue->asFloat32Value();
    }

    /**
     * Forwarded to AbstractStateValue::asFloat32() using this
     * node's state value.
     *
     * @see AbstractStateValue::asFloat32()
     */
    float asFloat32() const
    {
        return _stateValue->asFloat32();
    }

    /**
     * Forwarded to AbstractStateValue::asQuarkValue() using this
     * node's state value.
     *
     * @see AbstractStateValue::asQuarkValue()
     */
    const QuarkStateValue& asQuarkValue() const
    {
        return _stateValue->asQuarkValue();
    }

    /**
     * Forwarded to AbstractStateValue::asQuark() using this
     * node's state value.
     *
     * @see AbstractStateValue::asQuark()
     */
    Quark asQuark() const
    {
        return _stateValue->asQuark();
    }

    /**
     * Forwarded to AbstractStateValue::isSint32() using this
     * node's state value.
     *
     * @see AbstractStateValue::isSint32()
     */
    bool isSint32() const
    {
        return _stateValue->isSint32();
    }

    /**
     * Forwarded to AbstractStateValue::isSint64() using this
     * node's state value.
     *
     * @see AbstractStateValue::isSint64()
     */
    bool isSint64() const
    {
        return _stateValue->isSint64();
    }

    /**
     * Forwarded to AbstractStateValue::isUint32() using this
     * node's state value.
     *
     * @see AbstractStateValue::isUint32()
     */
    bool isUint32() const
    {
        return _stateValue->isUint32();
    }

    /**
     * Forwarded to AbstractStateValue::isUint64() using this
     * node's state value.
     *
     * @see AbstractStateValue::isUint64()
     */
    bool isUint64() const
    {
        return _stateValue->isUint64();
    }

    /**
     * Forwarded to AbstractStateValue::isFloat32() using this
     * node's state value.
     *
     * @see AbstractStateValue::isFloat32()
     */
    bool isFloat32() const
    {
        return _stateValue->isFloat32();
    }

    /**
     * Forwarded to AbstractStateValue::isQuark() using this
     * node's state value.
     *
     * @see AbstractStateValue::isQuark()
     */
    bool isQuark() const
    {
        return _stateValue->isQuark();
    }

    /**
     * Forwarded to AbstractStateValue::isNull() using this
     * node's state value.
     *
     * @see AbstractStateValue::isNull()
     */
    bool isNull() const
    {
        return _stateValue->isNull();
    }

    /**
     * Opposite of isNull().
     *
     * @see isNull()
     *
     * @returns True if this node's state value is not null
     */
    explicit operator bool() const
    {
        return !_stateValue->isNull();
    }

    /**
     * Returns a reference to the child node identified by \p quark,
     * creating a new one if it doesn't exist.
     *
     * @param quark Quark to look up
     * @returns     Child node with quark \p quark
     */
    StateNode& operator[](Quark quark);

    /**
     * Convenience method that gets the quark of string \p key and
     * calls operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param key Child key to look up
     * @returns   Child node with quark of \p key
     */
    StateNode& operator[](const std::string& key);

    /**
     * @see operator[](const std::string&)
     */
    StateNode& operator[](const char* key);

    /**
     * Convenience method that converts the integer \p key to a string,
     * gets the quark of this string and calls operator[](Quark)
     * with the result.
     *
     * @see operator[](Quark)
     *
     * @param key Child key to look up
     * @returns   Child node with quark of \p key (as a string)
     */
    StateNode& operator[](std::int64_t key);

    /**
     * @see operator[](std::int64_t)
     */
    StateNode& operator[](std::uint64_t key);

    /**
     * @see operator[](std::int64_t)
     */
    StateNode& operator[](std::int32_t key);

    /**
     * @see operator[](std::int64_t)
     */
    StateNode& operator[](std::uint32_t key);

    /**
     * @see operator[](std::int64_t)
     */
    StateNode& operator[](float key);

    /**
     * Convenience method that converts the signed integer event value
     * \p value to a string, gets the quark of this string and calls
     * operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param value Signed integer event value to look up
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const SintEventValue& value);

    /**
     * Convenience method that converts the unsigned integer event value
     * \p value to a string, gets the quark of this string and calls
     * operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param value Unsigned integer event value to look up
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const UintEventValue& value);

    /**
     * Convenience method that gets the string value of the string
     * event value \p value, gets the quark of this string and calls
     * operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param value String event value to look up
     * @returns     Child node with quark of \p value
     */
    StateNode& operator[](const StringEventValue& value);

    /**
     * Convenience method that, according to the type of \p value,
     * calls the appropriate method amongst
     * operator[](const SintEventValue&),
     * operator[](const UintEventValue&) and
     * operator[](const StringEventValue&).
     *
     * If the event value type is not one of the above, this method
     * returns a node with a quark corresponding to an empty string.
     *
     * This method has to check the event value's type and forward the
     * call to one of the aforementioned methods. Should you know the
     * event value's type in advance, prefer calling one of those
     * methods directly.
     *
     * @param value Event value to look up
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const AbstractEventValue& value);

    /**
     * Convenience method that converts the 32-bit signed integer
     * value of the state value \p value to a string, gets the quark
     * of this string and calls operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param value State value containing key
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const Sint32StateValue& value);

    /**
     * Convenience method that converts the 64-bit signed integer
     * value of the state value \p value to a string, gets the quark
     * of this string and calls operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param value State value containing key
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const Sint64StateValue& value);

    /**
     * Convenience method that converts the 32-bit unsigned integer
     * value of the state value \p value to a string, gets the quark
     * of this string and calls operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param value State value containing key
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const Uint32StateValue& value);

    /**
     * Convenience method that converts the 64-bit unsigned integer
     * value of the state value \p value to a string, gets the quark
     * of this string and calls operator[](Quark) with the result.
     *
     * @see operator[](Quark)
     *
     * @param value State value containing key
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const Uint64StateValue& value);

    /**
     * Convenience method that converts the 32-bit floating point
     * number value of the state value \p value to a string, gets
     * the quark of this string and calls operator[](Quark) with
     * the result.
     *
     * @see operator[](Quark)
     *
     * @param value State value containing key
     * @returns     Child node with quark of \p value (as a string)
     */
    StateNode& operator[](const Float32StateValue& value);

    /**
     * Convenience method that calls operator[](Quark) with the
     * quark value of \p value.
     *
     * @see operator[](Quark)
     *
     * @param value State value containing quark
     * @returns     Child node with quark of \p value
     */
    StateNode& operator[](const QuarkStateValue& value);

    /**
     * Returns an iterator to the first child node of this state node.
     *
     * If the state node has no children, the returned iterator will
     * be equal to end().
     *
     * @returns Iterator to the first child node
     */
    Iterator begin();

    /**
     * Returns an iterator to the child node following the last child
     * node of this state node.
     *
     * @returns Iterator to the child node following the last child node
     */
    Iterator end();

    /**
     * Verifies if this node has a child node identified by ­\p quark.
     *
     * @param quark Quark to look up
     * @returns     True if a child node exists with quark \p quark
     */
    bool hasChild(Quark quark) const;

    /**
     * Convenience method that gets the quark for the string \p key,
     * calls hasChild(Quark) and returns this result.
     *
     * @see hasChild(Quark)
     *
     * @param key Child key to look up
     * @returns   True if a child node exists with key \p key
     */
    bool hasChild(const std::string& key) const;

    /**
     * @see hasChild(const std::string&)
     */
    bool hasChild(const char* key) const;

    /**
     * Convenience method that converts the signed integer \p key to
     * a string, gets the quark for this string, calls hasChild(Quark)
     * and returns this result.
     *
     * @see hasChild(Quark)
     *
     * @param key Child key to look up
     * @returns   True if a child node exists with key \p key
     */
    bool hasChild(std::int64_t key) const;

    /**
     * Convenience method that converts the unsigned integer \p key to
     * a string, gets the quark for this string, calls hasChild(Quark)
     * and returns this result.
     *
     * @see hasChild(Quark)
     *
     * @param key Child key to look up
     * @returns   True if a child node exists with key \p key
     */
    bool hasChild(std::uint64_t key) const;

    /**
     * Convenience method that converts the signed integer event value
     * \p key to a string, gets the quark of this string, calls
     * hasChild(Quark) and returns this result.
     *
     * @see hasChild(Quark)
     *
     * @param key Signed integer event value to look up
     * @returns   True if a child node exists with key \p key
     */
    bool hasChild(const SintEventValue& key) const;

    /**
     * Convenience method that converts the unsigned integer event value
     * \p key to a string, gets the quark of this string, calls
     * hasChild(Quark) and returns this result.
     *
     * @see hasChild(Quark)
     *
     * @param key Unsigned integer event value to look up
     * @returns   True if a child node exists with key \p key
     */
    bool hasChild(const UintEventValue& key) const;

    /**
     * Convenience method that gets the quark of the string event value
     * \p key, calls hasChild(Quark) and returns this result.
     *
     * @see hasChild(Quark)
     *
     * @param key String event value to look up
     * @returns   True if a child node exists with key \p key
     */
    bool hasChild(const StringEventValue& key) const;

    /**
     * Convenience method that, according to the type of \p key,
     * calls the appropriate method amongst
     * hasChild(const SintEventValue&),
     * hasChild(const UintEventValue&) and
     * hasChild(const StringEventValue&).
     *
     * If the event value type is not one of the above, the return
     * value is undefined.
     *
     * This method has to check the event value's type and forward the
     * call to one of the aforementioned methods. Should you know the
     * event value's type in advance, prefer calling one of those
     * methods directly.
     *
     * @param key Event value to look up
     * @returns   True if a child node exists with key \p key
     */
    bool hasChild(const AbstractEventValue& key) const;

    /**
     * Returns how many children this node has, excluding null children.
     *
     * @see getAllChildrenCount() to get a count with null children
     *
     * @returns Number of non-null children this node has
     */
    std::size_t getChildrenCount() const;

    /**
     * Returns how many children this node has, including null children.
     *
     * @see getChildrenCount() to get a count without null children
     *
     * @returns Total number of children this node has
     */
    std::size_t getAllChildrenCount() const;

    /**
     * Assigns a state value to this node.
     *
     * All state values except AbstractStateValue should work here.
     * There's a specialized method for AbstractStateValue,
     * operator=(const AbstractStateValue&), which checks the concrete
     * state value type and calls this method after an appropriate
     * static cast.
     *
     * @param value State value to assign
     * @returns     This node
     */
    template<typename T>
    StateNode& operator=(const T& value);

    /**
     * Assigns a state value to this node.
     *
     * The type of \p value has to checked in order to cast it to
     * its proper class type and call the appropriate operator=()
     * method.
     *
     * Should you know the state value type in advance, please call
     * its dedicated operator=() method.
     *
     * @param value State value to assign
     * @returns     This node
     */
    StateNode& operator=(const AbstractStateValue& value);

    /**
     * Assigns the state value of \p node to this node (value is
     * copied).
     *
     * @param node Node containing state value to assign to this node
     * @returns    This node
     */
    StateNode& operator=(const StateNode& node);

    /**
     * Assigns the quark \p quark to this node.
     *
     * @param quark Quark
     * @returns     This node
     */
    StateNode& operator=(Quark quark);

    /**
     * Gets the string value quark of \p value and assigns it to this
     * node.
     *
     * @param value String to assign to this node
     * @returns     This node
     */
    StateNode& operator=(const std::string& value);

    /**
     * @see operator=(const std::string&)
     */
    StateNode& operator=(const char* value);

    /**
     * Assigns the 32-bit signed integer \p value to this node.
     *
     * @param value 32-bit signed integer to assign to this node
     * @returns     This node
     */
    StateNode& operator=(std::int32_t value);

    /**
     * Assigns the 32-bit unsigned integer \p value to this node.
     *
     * @param value 32-bit unsigned integer to assign to this node
     * @returns     This node
     */
    StateNode& operator=(std::uint32_t value);

    /**
     * Assigns the 64-bit signed integer \p value to this node.
     *
     * @param value 64-bit signed integer to assign to this node
     * @returns     This node
     */
    StateNode& operator=(std::int64_t value);

    /**
     * Assigns the 64-bit unsigned integer \p value to this node.
     *
     * @param value 64-bit unsigned integer to assign to this node
     * @returns     This node
     */
    StateNode& operator=(std::uint64_t value);

    /**
     * Assigns the single-precision floating point number \p value
     * to this node.
     *
     * @param value Floating point number to assign to this node
     * @returns     This node
     */
    StateNode& operator=(float value);

    /**
     * Assigns the 64-bit signed integer value of the signed integer
     * event value \p value to this node.
     *
     * @param value Signed integer event value to assign to this node
     * @returns     This node
     */
    StateNode& operator=(const SintEventValue& value);

    /**
     * Assigns the 64-bit unsigned integer value of the unsigned integer
     * event value \p value to this node.
     *
     * @param value Unsigned integer event value to assign to this node
     * @returns     This node
     */
    StateNode& operator=(const UintEventValue& value);

    /**
     * Assigns the single-precision floating point number of the
     * floating point number event value \p value to this node.
     *
     * The internal event value is casted to a single-precision
     * floating point number, so anything outside this range won't be
     * stored correctly.
     *
     * @param value Floating point number event value to assign to this node
     * @returns     This node
     */
    StateNode& operator=(const FloatEventValue& value);

    /**
     * Gets the string value quark of \p value and calls
     * operator=(Quark).
     *
     * @see operator=(Quark)
     *
     * @param value String event value to assign to this node
     * @returns     This node
     */
    StateNode& operator=(const StringEventValue& value);

    /**
     * Checks the type of \p value and calls the appropriate method
     * amongst operator=(const SintEventValue&),
     * operator=(const UintEventValue&),
     * operator=(const FloatEventValue&) and
     * operator=(const StringEventValue&).
     *
     * If the event value type is none of the above, no assignation
     * is performed.
     *
     * @param value String event value to assign to this node
     * @returns     This node
     */
    StateNode& operator=(const AbstractEventValue& value);

    /**
     * Nullifies this node's state value.
     *
     * @returns This node
     */
    StateNode& setNull();

    /**
     * Nullifies this node's state value and all its childen's,
     * recursively.
     *
     * @returns This node
     */
    StateNode& setNullRecursive();

    /**
     * Increments the state value of this node.
     *
     * If this node has no current state value or if its state value
     * type is not an integral type, the incrementation is not
     * performed.
     *
     * @returns This node
     */
    StateNode& operator++();

    /**
     * Decrements the state value of this node.
     *
     * If this node has no current state value or if its state value
     * type is not an integral type, the decrementation is not
     * performed.
     *
     * @returns This node
     */
    StateNode& operator--();

    /**
     * Adds \p inc to the state value of this node.
     *
     * If this node has no current state value or if its state value
     * type is not an integral type, the addition is not
     * performed.
     *
     * @param inc Value to add to the state value of this node
     * @returns   This node
     */
    StateNode& operator+=(std::int64_t inc);

    /**
     * Subtracts \p dec from the state value of this node.
     *
     * If this node has no current state value or if its state value
     * type is not an integral type, the subtraction is not
     * performed.
     *
     * @param dec Value to subtract from the state value of this node
     * @returns   This node
     */
    StateNode& operator-=(std::int64_t dec);

    /**
     * Compares two state nodes.
     *
     * @param rhs Other state node to compare
     * @returns   True if both state nodes are the same
     */
    bool operator==(const StateNode& rhs)
    {
        // nodes are not duplicated for the same ID in the same tree
        return this == std::addressof(rhs);
    }

    /**
     * Not operator==(const StateNode&).
     *
     * @see operator==(const StateNode&)
     */
    bool operator!=(const StateNode& rhs)
    {
        return !(*this == rhs);
    }

private:
    /**
     * Builds a state node.
     *
     * \p stateHistorySink is a (weak) pointer to the owning state
     * history sink, to which most of StateNode's method calls are
     * forwarded.
     *
     * The node is assigned node ID \p id.
     *
     * @param id               Node unique ID within the state tree
     * @param currentState     Owning CurrentState
     * @param beginTs          Initial begin timestamp of this node
     */
    StateNode(state_node_id_t id, CurrentState* currentState,
              timestamp_t beginTs);

    /**
     * Checks if a child node exists at all (be it null or not).
     *
     * @param quark Quark of child to look up
     * @returns     True if the child node with quark \p quark exists at all
     */
    bool childExists(quark_t quark) const
    {
        return _children.find(quark) != _children.end();
    }

    /**
     * Accepts a visitor \p visitor and makes it visit this node,
     * and then all its children (preorder).
     *
     * The quark of the visited node (which is unknown by this
     * node) must be provided.
     *
     * @param visitor Visitor to accept
     * @param quark   Quark of visited node
     */
    void acceptUpdate(AbstractStateNodeVisitor& visitor,
                      quark_t quark);

    /**
     * Const version of accept(AbstractStateNodeVisitor&, quark_t).
     *
     * @see accept(AbstractStateNodeVisitor&, quark_t)
     */
    void acceptRead(AbstractStateNodeVisitor& visitor,
                    quark_t quark) const;

    template<typename T>
    static void acceptImpl(T& stateNode, AbstractStateNodeVisitor& visitor,
                           quark_t quark);

    timestamp_t getCurrentSinkTimestamp();

private:
    // notifies the owning CurrentState of a state change
    void onStateChange(const AbstractStateValue& newValue) const;

    // node ID
    state_node_id_t _id;

    // state value
    AbstractStateValue::UP _stateValue;

    // current begin timestamp
    timestamp_t _beginTs;

    // children (quark -> state node) map
    std::unordered_map<quark_t, StateNode::UP> _children;

    // current state
    CurrentState* _currentState;
};

template<typename T>
StateNode& StateNode::operator=(const T& value)
{
    // encapsulate the new value in an AbstractStateValue
    auto newValue = AbstractStateValue::UP {new T{value}};

    // notify CurrentState
    onStateChange(*newValue);

    // update current begin timestamp
    this->_beginTs = this->getCurrentSinkTimestamp();

    // assign new value
    this->_stateValue = std::move(newValue);

    return *this;
}

}
}

#endif // _TIBEE_COMMON_STATENODE_HPP
