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
#ifndef _TIBEE_STATE_CURRENTSTATE_HPP
#define _TIBEE_STATE_CURRENTSTATE_HPP

#include <boost/functional/hash.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/BasicTypes.hpp"
#include "quark/QuarkDatabase.hpp"
#include "state/StateKey.hpp"
#include "state/StatePath.hpp"
#include "state/StateTree.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace state
{

/**
 * Current state.
 *
 * @author Francois Doray
 */
class CurrentState
{
public:
    CurrentState();
    ~CurrentState();

    void SetTimestamp(timestamp_t ts) {
        _ts = ts;
    }
    timestamp_t timestamp() const {
        return _ts;
    }

    quark::Quark Quark(const std::string& str);
    const std::string& String(quark::Quark quark) const;

    StateKey GetStateKey(const StatePath& path);
    StateKey GetStateKeyStr(const StatePathStr& pathStr);
    StateKey GetStateKey(StateKey root, const StatePath& subPath);

    void SetState(StateKey state, value::Value::UP value);
    void SetState(StateKey state, const StatePath& subPath, value::Value::UP value);
    void SetState(const StatePath& path, value::Value::UP value);

    const value::Value* GetStateValue(StateKey state);
    const value::Value* GetStateValue(StateKey state, const StatePath& subPath);
    const value::Value* GetStateValue(const StatePath& path);

    timestamp_t GetStateLastChange(StateKey state);
    timestamp_t GetStateLastChange(StateKey state, const StatePath& subPath);
    timestamp_t GetStateLastChange(const StatePath& path);

private:
    struct StateValue {
        StateValue();
        value::Value::UP value;
        timestamp_t since;
    };

    // Current timestamp.
    timestamp_t _ts;

    // Quark database.
    quark::QuarkDatabase<std::string> _quarks;

    // State tree.
    StateTree _stateTree;

    // State values.
    typedef std::unordered_map<size_t, StateValue> StateValues;
    StateValues _stateValues;
};

}
}

#endif // _TIBEE_STATE_STATEKEY_HPP
