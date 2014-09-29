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
#include "state/CurrentState.hpp"

#include <assert.h>

namespace tibee
{
namespace state
{

CurrentState::CurrentState() :
    _ts(0)
{
}

CurrentState::~CurrentState()
{
}

StateKey CurrentState::GetStateKey(const StatePath& statePath)
{
    auto look = _stateKeys.find(statePath);
    if (look != _stateKeys.end())
        return look->second;

    StateKey key(_stateValues.size());
    _stateValues.push_back(StateValue());
    _stateKeys[statePath] = key;

    return key;
}

const value::Value* CurrentState::GetStateValue(StateKey key) const
{
    assert(key.get() < _stateValues.size());
    return _stateValues[key.get()].value.get();
}

const value::Value* CurrentState::GetStateValue(const StatePath& path) const
{
    auto look = _stateKeys.find(path);
    if (look == _stateKeys.end())
        return nullptr;
    return GetStateValue(look->second);
}

timestamp_t CurrentState::GetStateLastChange(StateKey key) const
{
    assert(key.get() < _stateValues.size());
    return _stateValues[key.get()].since;
}

timestamp_t CurrentState::GetStateLastChange(const StatePath& path) const
{
    auto look = _stateKeys.find(path);
    if (look == _stateKeys.end())
        return -1;
    return GetStateLastChange(look->second);
}

void CurrentState::SetState(StateKey key, value::Value::UP value)
{
    assert(key.get() < _stateValues.size());

    _stateValues[key.get()].value = std::move(value);
    _stateValues[key.get()].since = _ts;
}

void CurrentState::SetState(const StatePath& path, value::Value::UP value)
{
    StateKey key = GetStateKey(path);
    SetState(key, std::move(value));
}

CurrentState::StateValue::StateValue() :
    since(0)
{
}

}
}
