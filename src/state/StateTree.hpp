/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#ifndef _TIBEE_STATE_STATETREE_HPP
#define _TIBEE_STATE_STATETREE_HPP

#include <iterator>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "quark/Quark.hpp"
#include "state/StateKey.hpp"
#include "state/StatePath.hpp"

namespace tibee
{
namespace state
{

/**
 * State tree.
 *
 * @author Francois Doray
 */
class StateTree
{
public:
    struct State;
    typedef std::unordered_map<quark::Quark, State*> States;
    struct State
    {
        StateKey key;
        States children;
    };

    StateTree();
    ~StateTree();

    StateKey GetStateKey(const StatePath& path);
    StateKey GetStateKey(StateKey root, const StatePath& subPath);

    typedef std::pair<quark::Quark, StateKey> QuarkStateKeyPair;
    class Iterator :
        public std::iterator<std::input_iterator_tag, QuarkStateKeyPair>
    {
    public:
        friend class StateTree;

        Iterator();
        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        const QuarkStateKeyPair& operator*() const;
        const QuarkStateKeyPair* operator->() const;

    private:
        Iterator(StateTree::States::const_iterator it);

        mutable QuarkStateKeyPair _currentPair;
        StateTree::States::const_iterator _it;
    };

    Iterator state_children_begin(StateKey key) const;
    Iterator state_children_end(StateKey key) const;

private:
    State* GetState(State* root, const StatePath& subPath);

    State _root;

    typedef std::vector<std::unique_ptr<State>> StateVector;
    StateVector _states;
};

}
}

#endif // _TIBEE_STATE_STATETREE_HPP
