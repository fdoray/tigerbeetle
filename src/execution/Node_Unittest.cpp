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
#include "execution/Node.hpp"
#include "gtest/gtest.h"

namespace tibee {
namespace execution {

TEST(Node, Node)
{
    const NodeId kTestId = 10;

    Node node(kTestId);
    EXPECT_EQ(kTestId, node.id());

    for (size_t i = 0; i < 5; ++i)
        node.AddChild(i);

    auto it = node.begin();
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_NE(it, node.end());
        EXPECT_EQ(i, *it);
        ++it;
    }
    EXPECT_EQ(node.end(), it);
}

}    // namespace execution
}    // namespace tibee
