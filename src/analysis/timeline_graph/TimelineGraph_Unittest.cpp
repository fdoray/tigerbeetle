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
#include "analysis/timeline_graph/TimelineGraph.hpp"
#include "analysis/timeline_graph/Node.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

TEST(TimelineGraph, TimelineGraph) {
    TimelineGraph graph;

    EXPECT_EQ(0, graph.size());
    EXPECT_TRUE(graph.empty());

    Node& a = graph.CreateNode();
    EXPECT_EQ(0, a.id());
    Node& b = graph.CreateNode();
    EXPECT_EQ(1, b.id());
    Node& c = graph.CreateNode();
    EXPECT_EQ(2, c.id());
    Node& d = graph.CreateNode();
    EXPECT_EQ(3, d.id());

    EXPECT_EQ(&a, &graph.GetNode(0));
    EXPECT_EQ(&b, &graph.GetNode(1));
    EXPECT_EQ(&c, &graph.GetNode(2));
    EXPECT_EQ(&d, &graph.GetNode(3));

    a.set_horizontal_child(b.id());
    b.set_horizontal_child(c.id());
    c.set_horizontal_child(d.id());
    d.set_horizontal_child(a.id());

    EXPECT_EQ(b.id(), graph.GetNode(a.id()).horizontal_child());
    EXPECT_EQ(c.id(), graph.GetNode(b.id()).horizontal_child());
    EXPECT_EQ(d.id(), graph.GetNode(c.id()).horizontal_child());
    EXPECT_EQ(a.id(), graph.GetNode(d.id()).horizontal_child());

    EXPECT_EQ(4, graph.size());
    EXPECT_FALSE(graph.empty());
}

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee
