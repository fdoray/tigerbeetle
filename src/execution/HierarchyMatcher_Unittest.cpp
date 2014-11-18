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

#include "base/Constants.hpp"
#include "execution/HierarchyMatcher.hpp"
#include "gtest/gtest.h"

namespace tibee {
namespace execution {

namespace
{

namespace pl = std::placeholders;

const uint64_t kSkipCost = 2;

uint64_t CharMatchCostFunc(
    const Graph& graph_a,
    const Graph& graph_b,
    const std::vector<char>& char_a,
    const std::vector<char>& char_b,
    NodeId a,
    NodeId b)
{
    return char_a[a] == char_b[b] ? 0 : kHugeCost;
}

std::string UniqueIdentifier(
    const Graph& graph_a,
    const Graph& graph_b,
    const std::vector<char>& char_a,
    const std::vector<char>& char_b,
    NodeId node_id,
    GraphPosition position)
{
    if (position == GraphPosition::LEFT_GRAPH)
      return std::string(1, char_a[node_id]);
    return std::string(1, char_b[node_id]);
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
    EXPECT_EQ(expected_cost, MatchGraphsHierarchical(
        a, b,
        std::bind(CharMatchCostFunc,
                  std::ref(a),
                  std::ref(b),
                  std::ref(char_a),
                  std::ref(char_b),
                  pl::_1,
                  pl::_2),
        std::bind(UniqueIdentifier,
                  std::ref(a),
                  std::ref(b),
                  std::ref(char_a),
                  std::ref(char_b),
                  pl::_1,
                  pl::_2),
        kSkipCost,
        &actual_nodes));

    // Check the result.
    std::vector<NodePair> expected_nodes_sorted(expected_nodes.begin(), expected_nodes.end());
    std::sort(expected_nodes_sorted.begin(), expected_nodes_sorted.end());
    std::sort(actual_nodes.begin(), actual_nodes.end());

    EXPECT_EQ(expected_nodes_sorted, actual_nodes);
}

}

TEST(HierarchyMatcher, EmptyGraphs)
{
    Graph a;
    Graph b;

    ExpectMatchChar(a, b, {}, {}, 0, {});
}

TEST(HierarchyMatcher, OneNodeGraphs) {
    Graph a;
    Graph b;
    Graph c;

    a.CreateNode();
    b.CreateNode();

    std::vector<char> charA = {'a'};
    std::vector<char> charB = {'a'};
    std::vector<char> charC = {};

    ExpectMatchChar(a, b, charA, charB, 0, {{0, 0}});
    ExpectMatchChar(a, c, charA, charC, kSkipCost, {});
    ExpectMatchChar(c, a, charC, charA, kSkipCost, {});
}

TEST(HierarchyMatcher, OneLevelHierarchyNoRepetitions) {
    Graph a;
    Graph b;

    Node& root_a = a.CreateNode();
    for (size_t i = 0; i < 3; ++i) {
        Node& node = a.CreateNode();
        root_a.AddChild(node.id());
    }

    Node& root_b = b.CreateNode();
    for (size_t i = 0; i < 3; ++i) {
        Node& node = b.CreateNode();
        root_b.AddChild(node.id());
    }

    std::vector<char> charA = {'a', 'b', 'c', 'd'};
    std::vector<char> charB = {'a', 'b', 'c', 'd'};

    ExpectMatchChar(
        a, b, charA, charB, 0,
        {{0, 0}, {1, 1}, {2, 2}, {3, 3}});
}

TEST(HierarchyMatcher, OneLevelHierarchyNotUnique) {
    Graph a;
    Graph b;

    Node& root_a = a.CreateNode();
    for (size_t i = 0; i < 5; ++i) {
        Node& node = a.CreateNode();
        root_a.AddChild(node.id());
    }

    Node& root_b = b.CreateNode();
    for (size_t i = 0; i < 5; ++i) {
        Node& node = b.CreateNode();
        root_b.AddChild(node.id());
    }

    std::vector<char> charA = {'a', 'b', 'c', 'c', 'd', 'c'};
    std::vector<char> charB = {'a', 'b', 'c', 'c', 'd', 'c'};

    ExpectMatchChar(
        a, b, charA, charB, 0,
        {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}});
}

TEST(HierarchyMatcher, OneLevelHierarchySimpleDifferences) {
    Graph a;
    Graph b;

    Node& root_a = a.CreateNode();
    for (size_t i = 0; i < 10; ++i) {
        Node& node = a.CreateNode();
        root_a.AddChild(node.id());
    }

    Node& root_b = b.CreateNode();
    for (size_t i = 0; i < 10; ++i) {
        Node& node = b.CreateNode();
        root_b.AddChild(node.id());
    }

    std::vector<char> charA = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};
    std::vector<char> charB = {'a', 'b', 'c', 'e', 'f', 'g', 'h', 'h', 'i', 'j', 'k'};

    ExpectMatchChar(
        a, b, charA, charB, 2 * kSkipCost,
        {{0, 0}, {1, 1}, {2, 2}, {4, 3}, {5, 4}, {6, 5}, {7, 6}, {8, 8}, {9, 9}, {10, 10}});
}

TEST(HierarchyMatcher, OneLevelHierarchySimpleDifferencesNotUnique) {
    Graph a;
    Graph b;

    Node& root_a = a.CreateNode();
    for (size_t i = 0; i < 10; ++i) {
        Node& node = a.CreateNode();
        root_a.AddChild(node.id());
    }

    Node& root_b = b.CreateNode();
    for (size_t i = 0; i < 6; ++i) {
        Node& node = b.CreateNode();
        root_b.AddChild(node.id());
    }
    //                               0    1    2    3    4    5    6    7    8    9
    std::vector<char> charA = {'a', 'b', 'c', 'c', 'd', 'd', 'e', 'e', 'f', 'f', 'g'};
    std::vector<char> charB = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};

    ExpectMatchChar(
        a, b, charA, charB, 4 * kSkipCost,
        {{0, 0}, {1, 1}, {2, 2}, {4, 3}, {6, 4}, {8, 5}, {10, 6}});
}

TEST(HierarchyMatcher, OneLevelHierarchyRepetitions) {
    Graph a;
    Graph b;

    Node& root_a = a.CreateNode();
    for (size_t i = 0; i < 50000; ++i) {
        Node& node = a.CreateNode();
        root_a.AddChild(node.id());
    }

    Node& root_b = b.CreateNode();
    for (size_t i = 0; i < 40000; ++i) {
        Node& node = b.CreateNode();
        root_b.AddChild(node.id());
    }

    std::vector<char> charA = {'a', 'b', 'c', 'd'};

    for (size_t i = 0; i < 12498; ++i) {
        charA.push_back('e');
        charA.push_back('f');
        charA.push_back('g');
        charA.push_back('h');
    }
    charA.push_back('a');
    charA.push_back('b');
    charA.push_back('c');
    charA.push_back('d');
    charA.push_back('e');

    std::vector<char> charB = {'a', 'b', 'c', 'd'};
    for (size_t i = 0; i < 9998; ++i) {
        charB.push_back('e');
        charB.push_back('f');
        charB.push_back('g');
        charB.push_back('h');
    }
    charB.push_back('h');
    charB.push_back('i');
    charB.push_back('j');
    charB.push_back('k');
    charB.push_back('l');

    std::vector<NodePair> expected_nodes;
    for (size_t i = 0; i < 39996; ++i)
        expected_nodes.push_back(NodePair(i, i));

    ExpectMatchChar(
        a, b, charA, charB, 10 * kSkipCost, expected_nodes);
}

TEST(HierarchyMatcher, OneLevelHierarchyTwoRepetitions) {
    Graph a;
    Graph b;

    Node& root_a = a.CreateNode();
    for (size_t i = 0; i < 20000; ++i) {
        Node& node = a.CreateNode();
        root_a.AddChild(node.id());
    }

    Node& root_b = b.CreateNode();
    for (size_t i = 0; i < 40000; ++i) {
        Node& node = b.CreateNode();
        root_b.AddChild(node.id());
    }

    std::vector<char> charA = {'a', 'b', 'c', 'd'};

    for (size_t i = 0; i < 2499; ++i) {
        charA.push_back('e');
        charA.push_back('f');
        charA.push_back('g');
        charA.push_back('h');
    }
    for (size_t i = 0; i < 2499; ++i) {
        charA.push_back('w');
        charA.push_back('x');
        charA.push_back('y');
        charA.push_back('z');
    }
    charA.push_back('a');
    charA.push_back('b');
    charA.push_back('c');
    charA.push_back('d');
    charA.push_back('e');

    std::vector<char> charB = {'a', 'b', 'c', 'd'};
    for (size_t i = 0; i < 4999; ++i) {
        charB.push_back('e');
        charB.push_back('f');
        charB.push_back('g');
        charB.push_back('h');
    }
    for (size_t i = 0; i < 4999; ++i) {
        charB.push_back('w');
        charB.push_back('x');
        charB.push_back('y');
        charB.push_back('z');
    }
    charB.push_back('h');
    charB.push_back('i');
    charB.push_back('j');
    charB.push_back('k');
    charB.push_back('l');

    std::vector<NodePair> expected_nodes;
    for (size_t i = 0; i < 4; ++i)
        expected_nodes.push_back(NodePair(i, i));
    for (size_t i = 4; i < 10000; ++i)
        expected_nodes.push_back(NodePair(i, i));
    for (size_t i = 10000; i < 19996; ++i)
        expected_nodes.push_back(NodePair(i, i + 10000));

    ExpectMatchChar(
        a, b, charA, charB, 10 * kSkipCost, expected_nodes);
}

}    // namespace execution
}    // namespace tibee
