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
#include "analysis/timeline_graph/Node.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

TEST(Node, Node)
{
    const NodeId kTestId = 4;

    Node node(kTestId);
    EXPECT_EQ(kTestId, node.id());
    EXPECT_EQ(kInvalidNodeId, node.horizontal_child());
    EXPECT_EQ(kInvalidNodeId, node.vertical_child());
    EXPECT_EQ(kInvalidNodeId,
                        node.child(Node::kChildHorizontal));
    EXPECT_EQ(kInvalidNodeId,
                        node.child(Node::kChildVertical));

    node.set_horizontal_child(3);
    node.set_vertical_child(4);
    EXPECT_EQ(3, node.horizontal_child());
    EXPECT_EQ(4, node.vertical_child());

    node.set_child(Node::kChildHorizontal, 5);
    node.set_child(Node::kChildVertical, 6);

    EXPECT_EQ(5, node.child(Node::kChildHorizontal));
    EXPECT_EQ(6, node.child(Node::kChildVertical));
}

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee
