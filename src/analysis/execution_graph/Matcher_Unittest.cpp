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
#include "analysis/execution_graph/Matcher.hpp"
#include "analysis/execution_graph/Graph.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

namespace {

namespace pl = std::placeholders;

uint64_t CharMatchCostFunc(
    const Graph& graph_a,
    const Graph& graph_b,
    const std::vector<char>& char_a,
    const std::vector<char>& char_b,
    NodeId a,
    NodeId b)
{
    return char_a[a] == char_b[b] ? 0 : 3;
}

void ExpectMatchChar(const Graph& a,
                     const Graph& b,
                     const std::vector<char>& char_a,
                     const std::vector<char>& char_b,
                     uint64_t expected_cost,
                     const std::vector<NodePair>& expected_nodes)
{
    // Match |graph_a| with |graph_b|.
    std::vector<NodePair> actual_nodes;
    EXPECT_EQ(expected_cost, MatchGraphs(
        a, b,
        std::bind(CharMatchCostFunc,
                  std::ref(a),
                  std::ref(b),
                  std::ref(char_a),
                  std::ref(char_b),
                  pl::_1,
                  pl::_2),
        2,
        &actual_nodes));

    EXPECT_EQ(expected_nodes, actual_nodes);

    // Match |graph_b| with |graph_a|.
    std::vector<NodePair> actual_nodes_reverse;
    EXPECT_EQ(expected_cost, MatchGraphs(
        b, a,
        std::bind(CharMatchCostFunc,
                  std::ref(b),
                  std::ref(a),
                  std::ref(char_b),
                  std::ref(char_a),
                  pl::_1,
                  pl::_2),
        2,
        &actual_nodes_reverse));

    std::vector<NodePair> expected_nodes_reverse;
    for (const NodePair& pair : expected_nodes) {
        expected_nodes_reverse.push_back(
            NodePair(pair.node_id_b(), pair.node_id_a()));
    }

    EXPECT_EQ(expected_nodes_reverse, actual_nodes_reverse);
}

}    // namespace

TEST(Matcher, EmptyGraphs) {
    Graph a;
    Graph b;

    ExpectMatchChar(a, b, {}, {}, 0, {});
}

TEST(Matcher, OneNodeGraphs) {
    Graph a;
    Graph b;
    Graph c;

    a.CreateNode();
    b.CreateNode();

    std::vector<char> charA = {'a'};
    std::vector<char> charB = {'a'};
    std::vector<char> charC = {};

    ExpectMatchChar(a, b, charA, charB, 0, {{0, 0}});
    ExpectMatchChar(a, c, charA, charB, 2, {});
}

TEST(Matcher, IdenticalGraphs) {
    //            a0
    //     b1            c4
    //   d2  e3        f5  g6 


    Graph a;
    Graph b;

    for (size_t i = 0; i < 7; ++i) {
        a.CreateNode();
        b.CreateNode();
    }

    // Graph A
    a.GetNode(0).AddChild(1);
    a.GetNode(0).AddChild(4);
    a.GetNode(1).AddChild(2);
    a.GetNode(1).AddChild(3);
    a.GetNode(4).AddChild(5);
    a.GetNode(4).AddChild(6);

    b.GetNode(0).AddChild(1);
    b.GetNode(0).AddChild(4);
    b.GetNode(1).AddChild(2);
    b.GetNode(1).AddChild(3);
    b.GetNode(4).AddChild(5);
    b.GetNode(4).AddChild(6);

    std::vector<char> charA = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
    std::vector<char> charB = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};

    ExpectMatchChar(
        a, b,
        charA, charB,
        0,
        {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}});
}

TEST(Matcher, NodeInsertion) {
    // Graph A:
    //            a0
    //       b1        c2
    //
    // Graph B:
    //            a0
    //       b1        c3
    //       d2

    Graph a;
    Graph b;

    for (size_t i = 0; i < 3; ++i) {
        a.CreateNode();
        b.CreateNode();
    }

    // Graph A
    a.GetNode(0).AddChild(1);
    a.GetNode(0).AddChild(2);

    // Graph B
    b.CreateNode();
    b.GetNode(0).AddChild(1);
    b.GetNode(0).AddChild(3);
    b.GetNode(1).AddChild(2);

    std::vector<char> charA = {'a', 'b', 'c'};
    std::vector<char> charB = {'a', 'b', 'd', 'c'};

    ExpectMatchChar(
        a, b,
        charA, charB,
        2,
        {{0, 0}, {1, 1}, {2, 3}});
}

TEST(Matcher, NodeMutation) {
    // Graph A:
    //            a0
    //       b1        c2
    //
    // Graph B:
    //            a0
    //       b1        d2

    Graph a;
    Graph b;

    for (size_t i = 0; i < 3; ++i) {
        a.CreateNode();
        b.CreateNode();
    }

    // Graph A
    a.GetNode(0).AddChild(1);
    a.GetNode(0).AddChild(2);

    // Graph B
    b.GetNode(0).AddChild(1);
    b.GetNode(0).AddChild(2);

    std::vector<char> charA = {'a', 'b', 'c'};
    std::vector<char> charB = {'a', 'b', 'd'};

    ExpectMatchChar(
        a, b,
        charA, charB,
        3,
        {{0, 0}, {1, 1}, {2, 2}});
}

TEST(Matcher, NodeMutationAndInsertion) {
    // Graph A:
    //            a0
    //       b1        c2
    //
    // Graph B:
    //            a0
    //       b1        d3
    //       e2

    Graph a;
    Graph b;

    for (size_t i = 0; i < 3; ++i) {
        a.CreateNode();
        b.CreateNode();
    }

    // Graph A
    a.GetNode(0).AddChild(1);
    a.GetNode(0).AddChild(2);

    // Graph B
    b.CreateNode();
    b.GetNode(0).AddChild(1);
    b.GetNode(0).AddChild(3);
    b.GetNode(1).AddChild(2);

    std::vector<char> charA = {'a', 'b', 'c'};
    std::vector<char> charB = {'a', 'b', 'd', 'e'};

    ExpectMatchChar(
        a, b,
        charA, charB,
        5,
        {{0, 0}, {1, 1}, {2, 3}});
}

}    // namespace execution_graph
}    // namespace analysis
}    // namespace tibee