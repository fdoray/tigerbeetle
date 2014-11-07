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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.    If not, see <http://www.gnu.org/licenses/>.
 */
#include "gtest/gtest.h"

#include "base/Constants.hpp"
#include "execution/ExecutionBuilder.hpp"
#include "quark/StringQuarkDatabase.hpp"
#include "value/MakeValue.hpp"

namespace tibee {
namespace execution {

using value::MakeValue;

TEST(ExecutionBuilder, SingleTask) {
    ExecutionBuilder builder;
    quark::StringQuarkDatabase quarks;
    builder.SetQuarks(&quarks);

    quark::Quark Q_NODE_TYPE = quarks.StrQuark(kNodeType);
    quark::Quark Q_START_TIME = quarks.StrQuark(kStartTime);
    quark::Quark Q_DURATION = quarks.StrQuark(kDuration);
    quark::Quark Q_DUMMY = quarks.StrQuark("dummy");
    quark::Quark Q_STACK_DEPTH = quarks.StrQuark(kStackDepth);

    // Build the graph.
    builder.SetTimestamp(0);
    builder.CreateExecution(1, "MyGraph");
    builder.SetProperty(1, Q_NODE_TYPE, MakeValue("*"));
    builder.SetProperty(1, Q_DUMMY, MakeValue("one"));
    
    builder.SetTimestamp(1);
    builder.PushStack(1);
    builder.SetProperty(1, Q_NODE_TYPE, MakeValue("a"));
    builder.SetProperty(1, Q_DUMMY, MakeValue("two"));

    builder.SetTimestamp(2);
    builder.PushStack(1);
    builder.SetProperty(1, Q_NODE_TYPE, MakeValue("b"));
    builder.SetProperty(1, Q_DUMMY, MakeValue("three"));

    builder.SetTimestamp(3);
    builder.PopStack(1);
    builder.SetProperty(1, Q_NODE_TYPE, MakeValue("c"));
    builder.SetProperty(1, Q_DUMMY, MakeValue("four"));

    builder.SetTimestamp(4);
    builder.PushStack(1);
    builder.SetProperty(1, Q_NODE_TYPE, MakeValue("d"));
    builder.SetProperty(1, Q_DUMMY, MakeValue("five"));

    builder.SetTimestamp(5);
    builder.PopStack(1);
    builder.SetProperty(1, Q_NODE_TYPE, MakeValue("e"));
    builder.SetProperty(1, Q_DUMMY, MakeValue("six"));

    builder.SetTimestamp(6);
    builder.PopStack(1);
    builder.SetProperty(1, Q_NODE_TYPE, MakeValue("f"));
    builder.SetProperty(1, Q_DUMMY, MakeValue("seven"));

    builder.SetTimestamp(7);
    builder.StopAllTimers();

    // Check the result.
    const auto& graph = (*builder.begin())->graph;
    const auto& properties = (*builder.begin())->properties;
    auto it = graph.preorder_begin();
    auto it_end = graph.preorder_end();

    // Node 0.
    EXPECT_EQ(0, it->id());
    EXPECT_EQ(0, it.Depth());
    EXPECT_EQ("*", properties.GetProperty(NodeStepKey(0, 0), Q_NODE_TYPE)->AsString());
    EXPECT_EQ(0, properties.GetProperty(NodeStepKey(0, 0), Q_START_TIME)->AsULong());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(0, 0), Q_DURATION)->AsULong());
    EXPECT_EQ("one", properties.GetProperty(NodeStepKey(0, 0), Q_DUMMY)->AsString());
    EXPECT_EQ(nullptr, properties.GetProperty(NodeStepKey(0, 0), Q_STACK_DEPTH));
    ++it;

    // Node 1.
    EXPECT_EQ(1, it->id());
    EXPECT_EQ(1, it.Depth());
    EXPECT_EQ("a", properties.GetProperty(NodeStepKey(1, 0), Q_NODE_TYPE)->AsString());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(1, 0), Q_START_TIME)->AsULong());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(1, 0), Q_DURATION)->AsULong());
    EXPECT_EQ("two", properties.GetProperty(NodeStepKey(1, 0), Q_DUMMY)->AsString());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(1, 0), Q_STACK_DEPTH)->AsUInteger());
    ++it;

    // Node 2.
    EXPECT_EQ(2, it->id());
    EXPECT_EQ(2, it.Depth());
    EXPECT_EQ("b", properties.GetProperty(NodeStepKey(2, 0), Q_NODE_TYPE)->AsString());
    EXPECT_EQ(2, properties.GetProperty(NodeStepKey(2, 0), Q_START_TIME)->AsULong());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(2, 0), Q_DURATION)->AsULong());
    EXPECT_EQ("three", properties.GetProperty(NodeStepKey(2, 0), Q_DUMMY)->AsString());
    EXPECT_EQ(2, properties.GetProperty(NodeStepKey(2, 0), Q_STACK_DEPTH)->AsUInteger());
    ++it;

    // Node 1, step 1
    EXPECT_EQ("c", properties.GetProperty(NodeStepKey(1, 1), Q_NODE_TYPE)->AsString());
    EXPECT_EQ(3, properties.GetProperty(NodeStepKey(1, 1), Q_START_TIME)->AsULong());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(1, 1), Q_DURATION)->AsULong());
    EXPECT_EQ("four", properties.GetProperty(NodeStepKey(1, 1), Q_DUMMY)->AsString());
    EXPECT_EQ(nullptr, properties.GetProperty(NodeStepKey(1, 1), Q_STACK_DEPTH));

    // Node 3.
    EXPECT_EQ(3, it->id());
    EXPECT_EQ(2, it.Depth());
    EXPECT_EQ("d", properties.GetProperty(NodeStepKey(3, 0), Q_NODE_TYPE)->AsString());
    EXPECT_EQ(4, properties.GetProperty(NodeStepKey(3, 0), Q_START_TIME)->AsULong());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(3, 0), Q_DURATION)->AsULong());
    EXPECT_EQ("five", properties.GetProperty(NodeStepKey(3, 0), Q_DUMMY)->AsString());
    EXPECT_EQ(2, properties.GetProperty(NodeStepKey(3, 0), Q_STACK_DEPTH)->AsUInteger());
    ++it;
    EXPECT_EQ(it_end, it);

    // Node 1, step 2
    EXPECT_EQ("e", properties.GetProperty(NodeStepKey(1, 2), Q_NODE_TYPE)->AsString());
    EXPECT_EQ(5, properties.GetProperty(NodeStepKey(1, 2), Q_START_TIME)->AsULong());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(1, 2), Q_DURATION)->AsULong());
    EXPECT_EQ("six", properties.GetProperty(NodeStepKey(1, 2), Q_DUMMY)->AsString());
    EXPECT_EQ(nullptr, properties.GetProperty(NodeStepKey(1, 2), Q_STACK_DEPTH));

    // Node 0, step 1
    EXPECT_EQ("f", properties.GetProperty(NodeStepKey(0, 1), Q_NODE_TYPE)->AsString());
    EXPECT_EQ(6, properties.GetProperty(NodeStepKey(0, 1), Q_START_TIME)->AsULong());
    EXPECT_EQ(1, properties.GetProperty(NodeStepKey(0, 1), Q_DURATION)->AsULong());
    EXPECT_EQ("seven", properties.GetProperty(NodeStepKey(0, 1), Q_DUMMY)->AsString());
    EXPECT_EQ(nullptr, properties.GetProperty(NodeStepKey(0, 1), Q_STACK_DEPTH));
}

}    // namespace execution
}    // namespace tibee
