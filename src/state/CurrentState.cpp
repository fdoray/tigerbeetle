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

quark::Quark CurrentState::Quark(const std::string& str)
{
    quark::Quark quark = _quarks.Insert(str);
    return quark;
}

const std::string& CurrentState::String(quark::Quark quark) const
{
    return _quarks.ValueOf(quark);
}

StateKey CurrentState::GetStateKey(const StatePath& path)
{
    return _stateTree.GetStateKey(path);
}

StateKey CurrentState::GetStateKeyStr(const StatePathStr& pathStr)
{
    StatePath path;
    path.reserve(pathStr.size());
    for (const auto& str : pathStr)
        path.push_back(Quark(str));
    return GetStateKey(path);
}

StateKey CurrentState::GetStateKey(StateKey root, const StatePath& subPath)
{
    return _stateTree.GetStateKey(root, subPath);
}

void CurrentState::SetState(StateKey state, value::Value::UP value)
{
    StateValue& stateValue = _stateValues[state.get()];
    stateValue.value = std::move(value);
    stateValue.since = _ts;
}

void CurrentState::SetState(StateKey state, const StatePath& subPath, value::Value::UP value)
{
    StateKey subPathKey = GetStateKey(state, subPath);
    SetState(subPathKey, std::move(value));
}

void CurrentState::SetState(const StatePath& path, value::Value::UP value)
{
    StateKey key = GetStateKey(path);
    SetState(key, std::move(value));
}

void CurrentState::NullState(StateKey state)
{
    SetState(state, value::Value::UP {});

    auto it = _stateTree.state_children_begin(state);
    auto it_end = _stateTree.state_children_end(state);
    for (; it != it_end; ++it)
        NullState(it->second);
}

void CurrentState::NullState(StateKey state, const StatePath& subPath)
{
    StateKey subPathKey = GetStateKey(state, subPath);
    NullState(subPathKey);
}

void CurrentState::NullState(const StatePath& path)
{
    StateKey key = GetStateKey(path);
    NullState(key);
}

const value::Value* CurrentState::GetStateValue(StateKey state)
{
    StateValue& stateValue = _stateValues[state.get()];
    return stateValue.value.get();
}

const value::Value* CurrentState::GetStateValue(StateKey state, const StatePath& subPath)
{
    StateKey subPathKey = GetStateKey(state, subPath);
    return GetStateValue(subPathKey);
}

const value::Value* CurrentState::GetStateValue(const StatePath& path)
{
    StateKey key = GetStateKey(path);
    return GetStateValue(key);
}

timestamp_t CurrentState::GetStateLastChange(StateKey state)
{
    StateValue& stateValue = _stateValues[state.get()];
    return stateValue.since;
}

timestamp_t CurrentState::GetStateLastChange(StateKey state, const StatePath& subPath)
{
    StateKey subPathKey = GetStateKey(state, subPath);
    return GetStateLastChange(subPathKey);
}

timestamp_t CurrentState::GetStateLastChange(const StatePath& path)
{
    StateKey key = GetStateKey(path);
    return GetStateLastChange(key);
}

CurrentState::StateValue::StateValue() :
    since(0)
{
}

}
}
