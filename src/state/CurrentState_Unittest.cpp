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
    uint32_t int_value = 0;
    const value::Value* value = nullptr;

    CurrentState currentState;

    currentState.SetTimestamp(1);
    currentState.SetState({"a", "b"}, value::Value::UP {new value::UIntValue(42)});
    currentState.SetTimestamp(2);

    value = currentState.GetStateValue({"a", "b"});
    ASSERT_NE(nullptr, value);
    EXPECT_TRUE(value->GetAsUInteger(&int_value));
    EXPECT_EQ(42, int_value);
    EXPECT_EQ(1, currentState.GetStateLastChange({"a", "b"}));

    currentState.SetTimestamp(3);
    currentState.SetState({"a"}, value::Value::UP {new value::UIntValue(1337)});
    currentState.SetTimestamp(4);

    value = currentState.GetStateValue(currentState.GetStateKey({"a"}));
    ASSERT_NE(nullptr, value);
    EXPECT_TRUE(value->GetAsUInteger(&int_value));
    EXPECT_EQ(1337, int_value);
    EXPECT_EQ(3, currentState.GetStateLastChange(currentState.GetStateKey({"a"})));

    value = currentState.GetStateValue({"a", "b"});
    ASSERT_NE(nullptr, value);
    EXPECT_TRUE(value->GetAsUInteger(&int_value));
    EXPECT_EQ(42, int_value);
    EXPECT_EQ(1, currentState.GetStateLastChange({"a", "b"}));
}

}  // namespace state
}  // namespace tibee
