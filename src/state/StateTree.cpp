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
#include "state/StateTree.hpp"

namespace tibee
{
namespace state
{

StateTree::StateTree()
{
}

StateTree::~StateTree()
{
}

StateKey StateTree::GetStateKey(const StatePath& path)
{
    return GetState(&_root, path)->key;
}

StateKey StateTree::GetStateKey(StateKey root, const StatePath& subPath)
{
    State* rootState = _states[root.get()].get();
    return GetState(rootState, subPath)->key;
}

StateTree::State* StateTree::GetState(State* root, const StatePath& subPath)
{
    State* currentState = root;
    for (quark::Quark quark : subPath)
    {
        auto look = currentState->children.find(quark.get());
        if (look == currentState->children.end())
        {
            std::unique_ptr<State> newState { new State };
            auto newStatePtr = newState.get();
            newStatePtr->key = StateKey(_states.size());

            _states.push_back(std::move(newState));

            currentState->children[quark.get()] = newStatePtr;
            currentState = newStatePtr;
        }
        else
        {
            currentState = look->second;
        }
    }

    return currentState;
}

}
}
