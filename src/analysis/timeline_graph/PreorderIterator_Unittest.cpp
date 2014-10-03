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
#include "analysis/timeline_graph/PreorderIterator.hpp"
#include "analysis/timeline_graph/TimelineGraph.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

namespace {

void ExpectTraversal(const std::vector<NodeId>& expected_nodes,
                     const std::vector<size_t>& expected_depths,
                     TimelineGraph& graph) {
    std::vector<NodeId> actual_nodes;
    std::vector<size_t> actual_depths;

    PreorderIterator it = graph.preorder_begin();
    PreorderIterator end = graph.preorder_end();

    for (; it != end; ++it) {
        actual_nodes.push_back(it->id());
        actual_depths.push_back(it.Depth());
        EXPECT_EQ(&graph.GetNode(it->id()), &(*it));
    }
    
    EXPECT_EQ(expected_nodes, actual_nodes);
    EXPECT_EQ(expected_depths, actual_depths);
}

}    // namespace

TEST(PreorderIterator, EmptyGraph) {
    TimelineGraph graph;

    std::vector<NodeId> expected_nodes = {};
    std::vector<NodeId> expected_depths = {};
    
    ExpectTraversal(expected_nodes, expected_depths, graph);
}

TEST(PreorderIterator, HorizontalGraph) {
    TimelineGraph graph;

    for (size_t i = 0; i < 11; ++i)
        graph.CreateNode();

    for (size_t i = 0; i < 10; ++i)
        graph.GetNode(i).set_horizontal_child(i + 1);

    std::vector<NodeId> expected_nodes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<size_t> expected_depths = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    ExpectTraversal(expected_nodes, expected_depths, graph);
}

TEST(PreorderIterator, VerticalGraph) {
    TimelineGraph graph;

    for (size_t i = 0; i < 11; ++i)
        graph.CreateNode();

    for (size_t i = 0; i < 10; ++i)
        graph.GetNode(i).set_vertical_child(i + 1);

    std::vector<NodeId> expected_nodes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<size_t> expected_depths = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    ExpectTraversal(expected_nodes, expected_depths, graph);
}

TEST(PreorderIterator, ZigzagGraph) {
    TimelineGraph graph;

    for (size_t i = 0; i < 11; ++i)
        graph.CreateNode();

    for (size_t i = 0; i < 10; ++i) {
        if (i % 2 == 0)
            graph.GetNode(i).set_vertical_child(i + 1);
        else
            graph.GetNode(i).set_horizontal_child(i + 1);
    }

    std::vector<NodeId> expected_nodes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<size_t> expected_depths = {
        0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
    
    ExpectTraversal(expected_nodes, expected_depths, graph);
}

TEST(PreorderIterator, SimpleTreeGraph) {
    TimelineGraph graph;

    for (size_t i = 0; i < 11; ++i)
        graph.CreateNode();

    graph.GetNode(0).set_horizontal_child(1);
    graph.GetNode(0).set_vertical_child(2);

    graph.GetNode(1).set_horizontal_child(3);
    graph.GetNode(1).set_vertical_child(4);

    graph.GetNode(2).set_horizontal_child(5);

    graph.GetNode(3).set_horizontal_child(6);
    graph.GetNode(3).set_vertical_child(7);

    graph.GetNode(4).set_vertical_child(8);

    graph.GetNode(5).set_vertical_child(9);

    graph.GetNode(8).set_vertical_child(10);

    std::vector<NodeId> expected_nodes = {
        0, 2, 5, 9, 1, 4, 8, 10, 3, 7, 6};
    std::vector<size_t> expected_depths = {
        0, 1, 1, 2, 0, 1, 2, 3, 0, 1, 0};
    
    ExpectTraversal(expected_nodes, expected_depths, graph);
}

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee
