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
#ifndef _TIBEE_COMMON_CURRENTSTATE_HPP
#define _TIBEE_COMMON_CURRENTSTATE_HPP

#include <memory>
#include <cstddef>
#include <boost/utility.hpp>

#include <common/quark/Quark.hpp>
#include <common/quark/QuarkDatabase.hpp>
#include <common/state/NullStateValue.hpp>
#include <common/BasicTypes.hpp>


namespace tibee
{
namespace common
{

class StateNode;

/**
 * Current state (during a state history construction).
 *
 * An object of this class is the main one any state provider have
 * access to. They cannot open or close a state history, but are able
 * to set state values of different paths, triggering interval
 * creation/recording behind the scenes.
 *
 * @author Philippe Proulx
 */
class CurrentState final :
    boost::noncopyable
{
public:
    friend StateNode;

    CurrentState();
    ~CurrentState();

    /**
     * Returns a quark for a given string, created if needed.
     *
     * The quark will always be the same for the same string.
     *
     * @param string String for which to get the quark
     * @returns      Quark for given string
     */
    Quark getQuark(const std::string& subpath) const;

    /**
     * Returns the string associated with quark \p quark or
     * throws ex::WrongQuark if no such string exists.
     *
     * @returns String associated with quark \p quark
     */
    const std::string& getString(Quark quark) const;

    /**
     * Sets the history current timestamp. Timestamps should be set
     * in ascending order.
     *
     * @param ts Current timestamp
     */
    void setCurrentTimestamp(timestamp_t ts)
    {
        assert(ts >= _ts);

        _ts = ts;
    }

    /**
     * Returns the current history timestamp.
     *
     * @returns Current history timestamp
     */
    timestamp_t getCurrentTimestamp() const
    {
        return _ts;
    }

    /**
     * Returns the root of the state tree.
     *
     * @returns State tree root node
     */
    StateNode& getRoot();

    /**
     * Returns a null state value.
     *
     * @returns Null state value
     */
     const NullStateValue& getNull() const;

    /**
     * Builds a new state node, with a fresh, unused unique node ID.
     *
     * @returns Fresh state node
     */
    std::unique_ptr<StateNode> buildStateNode();

private:
    void onStateChange(const StateNode& stateNode,
                       const AbstractStateValue& newValue);

    // string database for state paths
    std::unique_ptr<QuarkDatabase<std::string>> _stringDb;

    // current timestamp
    timestamp_t _ts;

    // root state node
    std::unique_ptr<StateNode> _root;

    // Null state value
    NullStateValue::UP _null;

    // next state node unique ID to assign
    state_node_id_t _nextNodeId;
};

}
}

#endif // _TIBEE_COMMON_CURRENTSTATE_HPP
