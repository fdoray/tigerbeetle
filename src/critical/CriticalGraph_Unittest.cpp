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
#include <algorithm>

#include "gtest/gtest.h"
#include "critical/CriticalGraph.hpp"
#include "critical/CriticalPath.hpp"

namespace tibee {
namespace critical {

TEST(CriticalGraph, CriticalGraph)
{
    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    nodes.push_back(graph.CreateNode(0, 2)); // 0
    nodes.push_back(graph.CreateNode(0, 3)); // 1
    nodes.push_back(graph.CreateNode(1, 2)); // 2
    nodes.push_back(graph.CreateNode(1, 3)); // 3
    nodes.push_back(graph.CreateNode(1, 4)); // 4
    nodes.push_back(graph.CreateNode(2, 4)); // 5
    nodes.push_back(graph.CreateNode(2, 6)); // 6
    nodes.push_back(graph.CreateNode(3, 4)); // 7
    nodes.push_back(graph.CreateNode(3, 5)); // 8
    nodes.push_back(graph.CreateNode(4, 4)); // 9
    nodes.push_back(graph.CreateNode(4, 6)); // 10
    nodes.push_back(graph.CreateNode(5, 2)); // 11
    nodes.push_back(graph.CreateNode(5, 3)); // 12
    nodes.push_back(graph.CreateNode(5, 4)); // 13
    nodes.push_back(graph.CreateNode(6, 1)); // 14
    nodes.push_back(graph.CreateNode(6, 2)); // 15
    nodes.push_back(graph.CreateNode(6, 3)); // 16
    nodes.push_back(graph.CreateNode(6, 4)); // 17
    nodes.push_back(graph.CreateNode(7, 3)); // 18
    nodes.push_back(graph.CreateNode(7, 4)); // 19
    nodes.push_back(graph.CreateNode(8, 1)); // 20
    nodes.push_back(graph.CreateNode(8, 2)); // 21
    nodes.push_back(graph.CreateNode(9, 2)); // 22
    nodes.push_back(graph.CreateNode(9, 3)); // 23
    nodes.push_back(graph.CreateNode(10, 5)); // 24

    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[11], nodes[15]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[14], nodes[18]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[19], nodes[22]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[1], nodes[3]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[4], nodes[5]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[9], nodes[13]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[12], nodes[16]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[17], nodes[21]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[20], nodes[23]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[6], nodes[10]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRun, nodes[8], nodes[24]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kBlock, nodes[15], nodes[19]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kBlock, nodes[3], nodes[12]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kBlock, nodes[7], nodes[9]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kBlock, nodes[16], nodes[20]);

    graph.CreateVerticalEdge(nodes[15], nodes[14]);
    graph.CreateVerticalEdge(nodes[18], nodes[19]);
    graph.CreateVerticalEdge(nodes[3], nodes[4]);
    graph.CreateVerticalEdge(nodes[5], nodes[6]);
    graph.CreateVerticalEdge(nodes[7], nodes[8]);
    graph.CreateVerticalEdge(nodes[10], nodes[9]);
    graph.CreateVerticalEdge(nodes[13], nodes[12]);
    graph.CreateVerticalEdge(nodes[16], nodes[17]);
    graph.CreateVerticalEdge(nodes[21], nodes[20]);

    // Extract critical path.
    CriticalPath path;
    EXPECT_TRUE(graph.ComputeCriticalPath(nodes[1], nodes[23], &path));

    ASSERT_EQ(7, path.size());

    auto it = path.begin();
    auto it_end = path.end();

    ASSERT_NE(it, it_end);
    EXPECT_EQ(0, it->ts);
    EXPECT_EQ(3, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(1, it->ts);
    EXPECT_EQ(4, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(2, it->ts);
    EXPECT_EQ(6, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(4, it->ts);
    EXPECT_EQ(4, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(5, it->ts);
    EXPECT_EQ(3, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(6, it->ts);
    EXPECT_EQ(4, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(8, it->ts);
    EXPECT_EQ(1, it->tid);
    ++it;

    EXPECT_EQ(it, it_end);
}


}    // namespace critical
}    // namespace tibee