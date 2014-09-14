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
#include <common/state/AbstractStateValue.hpp>
#include <common/BasicTypes.hpp>


namespace tibee
{
namespace common
{

class StateHistorySink;
class StateNode;

/**
 * Current state (during a state history construction); façade of a
 * StateHistorySink for state providers.
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
    friend class StateHistorySink;

public:
    /**
     * @see StateHistorySink::getQuark()
     */
    Quark getQuark(const std::string& subpath) const;

    /**
     * @see StateHistorySink::getString()
     */
    const std::string& getString(Quark quark) const;

    /**
     * @see StateHistorySink::getStateChangesCount()
     */
    std::size_t getStateChangesCount() const;

    /**
     * @see StateHistorySink::getNodesCount()
     */
    std::size_t getNodesCount() const;

    /**
     * @see StateHistorySink::getRoot()
     */
    StateNode& getRoot();

private:
    // only StateHistorySink may build a CurrentState object
    CurrentState(StateHistorySink* sink);

private:
    StateHistorySink* _sink;
};

}
}

#endif // _TIBEE_COMMON_CURRENTSTATE_HPP
