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
#ifndef _TIBEE_COMMON_STATEHISTORYSINK_HPP
#define _TIBEE_COMMON_STATEHISTORYSINK_HPP

#include <cassert>
#include <memory>
#include <cstdint>
#include <array>
#include <functional>
#include <unordered_map>
#include <boost/utility.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <delorean/HistoryFileSink.hpp>
#include <delorean/interval/AbstractInterval.hpp>

#include <common/BasicTypes.hpp>
#include <common/quark/Quark.hpp>
#include <common/state/AbstractStateValue.hpp>
#include <common/state/NullStateValue.hpp>
#include <common/state/CurrentState.hpp>
#include <common/state/StateNode.hpp>
#include <common/state/AbstractStateValue.hpp>

namespace tibee
{
namespace common
{

class StateNode;

/**
 * A state history sink.
 *
 * An object of this class must be used to write a state history on
 * disk. A state history comprises a few files: two string databases
 * (one for paths and the other for state values) and a history of
 * state intervals.
 *
 * @author Philippe Proulx
 */
class StateHistorySink :
    boost::noncopyable
{
    // mutual friendship FTW
    friend StateNode;

public:
    /**
     * Builds a state history sink.
     *
     * The current history timestamp is initialized with 0.
     *
     * @param stringDbPath  Path to string database file (to be created)
     * @param nodesMapPath   Path to map of state nodes IDs to paths (to be created)
     * @param historyPath    Path to history file (to be created)
     * @param beginTs        Begin timestamp to use
     */
    StateHistorySink(const boost::filesystem::path& stringDbPath,
                     const boost::filesystem::path& nodesMapPath,
                     const boost::filesystem::path& historyPath,
                     timestamp_t beginTs);

    ~StateHistorySink();

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
     * Closes this state history sink, effectively closing all opened
     * files, flushing various internal databases to disk and marking it
     * as closed.
     *
     * All state nodes of the state tree are written as intervals, with
     * the current history timestamp as their end timestamp.
     */
    void close();

    /**
     * Returns a quark for a given string, created if needed.
     *
     * The quark will always be the same for the same string.
     *
     * @param string String for which to get the quark
     * @returns      Quark for given string
     */
    Quark getQuark(const std::string& string);

    /**
     * Returns the string associated with quark \p quark or
     * throws ex::WrongQuark if no such string exists.
     *
     * @returns String associated with quark \p quark
     */
    const std::string& getString(Quark quark) const;

    /**
     * Returns a reference to the "current state", which is an adapter
     * that state providers may use to access this sink without having
     * access to irrelevant methods.
     *
     * The current state remains valid/usable as long as this sink is.
     *
     * @returns Current state reference
     */
    CurrentState& getCurrentState()
    {
        return _currentState;
    }

    /**
     * Returns the number of state changes so far.
     *
     * @returns State changes count
     */
    std::size_t getStateChangesCount() const
    {
        return _stateChangesCount;
    }

    /**
     * Returns the number of state nodes in the state tree so far,
     * including the root node.
     *
     * @returns Number of state nodes in the state tree
     */
    std::size_t getNodesCount() const;

    /**
     * Returns the root of the state tree.
     *
     * @returns State tree root node
     */
    StateNode& getRoot()
    {
        return *_root;
    }

    /**
     * Returns a null state value.
     *
     * @returns Null state value
     */
    const NullStateValue& getNull() const
    {
        return *_null;
    }

private:
    // a string database
    typedef boost::bimaps::bimap<
        boost::bimaps::unordered_set_of<std::string>,
        boost::bimaps::unordered_set_of<quark_t>
    > StringDb;

    // a (state node -> delorean interval) translator
    typedef std::function<delo::AbstractInterval* (const StateNode&)> Translator;

private:
    void initTranslators();
    void open();
    void writeStringDb(const StringDb& stringDb,
                       const boost::filesystem::path& path);
    quark_t getQuark(StringDb& stringDb, const std::string& value,
                     quark_t& nextQuark);

    static const std::string& getQuarkString(const StringDb& stringDb,
                                             quark_t quark);

    /**
     * Writes the map of state node IDs to paths to a file.
     */
    void writeNodesMap() const;

    /**
     * Builds a new state node, with a fresh, unused unique node ID.
     *
     * @returns Fresh state node
     */
    StateNode::UP buildStateNode();

    /**
     * Called by state nodes when an interval needs to be written.
     *
     * If the node has no current state value, no interval is written.
     *
     * @param node Node for which an interval needs to be created
     */
    void writeInterval(const StateNode& node);

    /**
     * Nullifies all nodes of the state tree.
     */
    void nullifyAllNodes();

private:
    // paths to files to create
    boost::filesystem::path _stringDbPath;
    boost::filesystem::path _nodesMapPath;
    boost::filesystem::path _historyPath;

    // provided begin timestamp
    timestamp_t _beginTs;

    // current timestamp
    timestamp_t _ts;

    // open state
    bool _open;

    // string database for state paths
    StringDb _stringDb;

    // current state path quark
    quark_t _nextPathQuark;

    // current state value quark
    quark_t _nextStrValueQuark;

    // next state node unique ID to assign
    state_node_id_t _nextNodeId;

    // root state node
    StateNode::UP _root;

    // (state value -> delorean interval) translators
    std::array<Translator, 16> _translators;

    // current state adapter for state providers
    CurrentState _currentState;

    // interval history sink
    std::unique_ptr<delo::HistoryFileSink> _intervalFileSink;

    // count of state changes so far
    std::size_t _stateChangesCount;

    // Null state value
    NullStateValue::UP _null;
};

}
}

#endif // _TIBEE_COMMON_STATEHISTORYSINK_HPP
