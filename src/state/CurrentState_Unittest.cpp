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
#include "quark/StringQuarkDatabase.hpp"
#include "state/CurrentState.hpp"

namespace tibee
{
namespace state
{

TEST(CurrentState, Timestamp)
{
    quark::StringQuarkDatabase quarks;
    CurrentState currentState(nullptr, &quarks);
    EXPECT_EQ(0, currentState.timestamp());

    currentState.SetTimestamp(1);
    EXPECT_EQ(1, currentState.timestamp());
}

TEST(CurrentState, AttributeChanges)
{
    quark::StringQuarkDatabase quarks;
    CurrentState currentState(nullptr, &quarks);

    AttributeKey abKey = currentState.GetAttributeKeyStr({"a", "b"});

    currentState.SetTimestamp(1);
    currentState.SetAttribute(abKey, value::Value::UP {new value::UIntValue(42)});

    EXPECT_EQ(42, currentState.GetAttributeValue(abKey)->AsUInteger());
    EXPECT_EQ(1, currentState.GetAttributeLastChange(abKey));

    AttributeKey aKey = currentState.GetAttributeKeyStr({"a"});
    currentState.SetTimestamp(3);
    currentState.SetAttribute(
        aKey,
        {currentState.Quark("b"), currentState.Quark("c")},
        value::Value::UP {new value::UIntValue(1337)});

    AttributeKey abcKey = currentState.GetAttributeKeyStr({"a", "b", "c"});
    EXPECT_EQ(1337, currentState.GetAttributeValue(abcKey)->AsUInteger());
    EXPECT_EQ(3, currentState.GetAttributeLastChange(abcKey));

    EXPECT_EQ(42, currentState.GetAttributeValue(
        aKey,
        {currentState.Quark("b")})->AsUInteger());
    EXPECT_EQ(1, currentState.GetAttributeLastChange(abKey));
}

TEST(CurrentState, NullAttribute)
{
    quark::StringQuarkDatabase quarks;
    CurrentState currentState(nullptr, &quarks);

    AttributeKey aKey = currentState.GetAttributeKeyStr({"a"});
    AttributeKey abKey = currentState.GetAttributeKeyStr({"a", "b"});
    AttributeKey abcKey = currentState.GetAttributeKeyStr({"a", "b", "c"});
    AttributeKey abdKey = currentState.GetAttributeKeyStr({"a", "b", "d"});
    AttributeKey abdeKey = currentState.GetAttributeKeyStr({"a", "b", "d", "e"});

    currentState.SetAttribute(aKey, value::Value::UP {new value::UIntValue(42)});
    currentState.SetAttribute(abKey, value::Value::UP {new value::UIntValue(42)});
    currentState.SetAttribute(abcKey, value::Value::UP {new value::UIntValue(42)});
    currentState.SetAttribute(abdKey, value::Value::UP {new value::UIntValue(42)});
    currentState.SetAttribute(abdeKey, value::Value::UP {new value::UIntValue(42)});

    currentState.NullAttribute(abKey);

    EXPECT_EQ(42, currentState.GetAttributeValue(aKey)->AsUInteger());
    EXPECT_EQ(nullptr, currentState.GetAttributeValue(abKey));
    EXPECT_EQ(nullptr, currentState.GetAttributeValue(abcKey));
    EXPECT_EQ(nullptr, currentState.GetAttributeValue(abdKey));
    EXPECT_EQ(nullptr, currentState.GetAttributeValue(abdeKey));
}

}  // namespace state
}  // namespace tibee
