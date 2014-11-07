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
#include "execution/PreorderIterator.hpp"
#include "execution/Graph.hpp"
#include "gtest/gtest.h"

namespace tibee {
namespace execution {

namespace {

void ExpectTraversal(const std::vector<NodeId>& expected_nodes,
                     const std::vector<size_t>& expected_depths,
                     const std::vector<size_t>& expected_child_index,
                     Graph& graph) {
    std::vector<NodeId> actual_nodes;
    std::vector<size_t> actual_depths;
    std::vector<size_t> actual_child_index;

    PreorderIterator it = graph.preorder_begin();
    PreorderIterator end = graph.preorder_end();

    for (; it != end; ++it) {
        actual_nodes.push_back(it->id());
        actual_depths.push_back(it.Depth());
        actual_child_index.push_back(it.ChildIndex());
        EXPECT_EQ(&graph.GetNode(it->id()), &(*it));
    }
    
    EXPECT_EQ(expected_nodes, actual_nodes);
    EXPECT_EQ(expected_depths, actual_depths);
    EXPECT_EQ(expected_child_index, actual_child_index);
}

}    // namespace

TEST(PreorderIterator, EmptyGraph) {
    Graph graph;

    std::vector<NodeId> expected_nodes = {};
    std::vector<size_t> expected_depths = {};
    std::vector<size_t> expected_child_index = {};
    
    ExpectTraversal(expected_nodes, expected_depths, expected_child_index, graph);
}


TEST(PreorderIterator, SimpleGraph) {
    Graph graph;

    Node& root = graph.CreateNode();
    for (size_t i = 0; i < 10; ++i)
        root.AddChild(graph.CreateNode().id());

    std::vector<NodeId> expected_nodes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<size_t> expected_depths = {
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<size_t> expected_child_index = {
        0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    ExpectTraversal(expected_nodes, expected_depths, expected_child_index, graph);
}

TEST(PreorderIterator, VerticalGraph) {
    Graph graph;

    Node* lastNode = &graph.CreateNode();
    for (size_t i = 0; i < 10; ++i) {
        Node* newNode = &graph.CreateNode();
        lastNode->AddChild(newNode->id());
        lastNode = newNode;
    }

    std::vector<NodeId> expected_nodes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<size_t> expected_depths = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<size_t> expected_child_index = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    ExpectTraversal(expected_nodes, expected_depths, expected_child_index, graph);
}

TEST(PreorderIterator, ComplexGraph) {
    Graph graph;

    for (size_t i = 0; i < 10; ++i)
        graph.CreateNode();

    graph.GetNode(0).AddChild(1);
    graph.GetNode(0).AddChild(6);
    graph.GetNode(0).AddChild(7);

    graph.GetNode(1).AddChild(2);
    graph.GetNode(1).AddChild(3);
    graph.GetNode(1).AddChild(4);
    graph.GetNode(1).AddChild(5);

    graph.GetNode(7).AddChild(8);

    graph.GetNode(8).AddChild(9);

    std::vector<NodeId> expected_nodes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<size_t> expected_depths = {
        0, 1, 2, 2, 2, 2, 1, 1, 2, 3};
    std::vector<size_t> expected_child_index = {
        0, 0, 0, 1, 2, 3, 1, 2, 0, 0};
    
    ExpectTraversal(expected_nodes, expected_depths, expected_child_index, graph);
}

}    // namespace execution
}    // namespace tibee
