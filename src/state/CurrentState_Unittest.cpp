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
#include "gtest/gtest.h"
#include "state/CurrentState.hpp"

namespace tibee
{
namespace state
{

TEST(CurrentState, Timestamp)
{
    CurrentState currentState;
    EXPECT_EQ(0, currentState.timestamp());

    currentState.SetTimestamp(1);
    EXPECT_EQ(1, currentState.timestamp());
}

TEST(CurrentState, StateChanges)
{
    CurrentState currentState;

    StateKey abKey = currentState.GetStateKeyStr({"a", "b"});

    currentState.SetTimestamp(1);
    currentState.SetState(abKey, value::Value::UP {new value::UIntValue(42)});

    EXPECT_EQ(42, currentState.GetStateValue(abKey)->AsUInteger());
    EXPECT_EQ(1, currentState.GetStateLastChange(abKey));

    StateKey aKey = currentState.GetStateKeyStr({"a"});
    currentState.SetTimestamp(3);
    currentState.SetState(
        aKey,
        {currentState.Quark("b"), currentState.Quark("c")},
        value::Value::UP {new value::UIntValue(1337)});

    StateKey abcKey = currentState.GetStateKeyStr({"a", "b", "c"});
    EXPECT_EQ(1337, currentState.GetStateValue(abcKey)->AsUInteger());
    EXPECT_EQ(3, currentState.GetStateLastChange(abcKey));

    EXPECT_EQ(42, currentState.GetStateValue(
        aKey,
        {currentState.Quark("b")})->AsUInteger());
    EXPECT_EQ(1, currentState.GetStateLastChange(abKey));
}

}  // namespace state
}  // namespace tibee
