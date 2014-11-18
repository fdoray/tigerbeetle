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
#include <fstream>
#include <string>

#include "base/Constants.hpp"
#include "execution/HierarchyMatcher.hpp"
#include "gtest/gtest.h"

namespace tibee {
namespace execution {

namespace
{

namespace pl = std::placeholders;

const uint64_t kSkipCost = 2;

uint64_t StrMatchCostFunc(
    const Graph& graph_a,
    const Graph& graph_b,
    const std::vector<std::string>& str_a,
    const std::vector<std::string>& str_b,
    NodeId a,
    NodeId b)
{
    return str_a[a] == str_b[b] ? 0 : kHugeCost;
}

std::string UniqueIdentifier(
    const Graph& graph_a,
    const Graph& graph_b,
    const std::vector<std::string>& str_a,
    const std::vector<std::string>& str_b,
    NodeId node_id,
    GraphPosition position)
{
    if (position == GraphPosition::LEFT_GRAPH)
      return str_a[node_id];
    return str_b[node_id];
}

void ExpectMatchStr(const Graph& a,
                    const Graph& b,
                    const std::vector<std::string>& str_a,
                    const std::vector<std::string>& str_b,
                    uint64_t expected_cost,
                    const std::vector<NodePair>& expected_nodes)
{
    // Match |graph_a| with |graph_b|.
    std::vector<NodePair> actual_nodes;
    EXPECT_EQ(expected_cost, MatchGraphsHierarchical(
        a, b,
        std::bind(StrMatchCostFunc,
                  std::ref(a),
                  std::ref(b),
                  std::ref(str_a),
                  std::ref(str_b),
                  pl::_1,
                  pl::_2),
        std::bind(UniqueIdentifier,
                  std::ref(a),
                  std::ref(b),
                  std::ref(str_a),
                  std::ref(str_b),
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

    ExpectMatchStr(a, b, {}, {}, 0, {});
}

TEST(HierarchyMatcher, OneNodeGraphs) {
    Graph a;
    Graph b;
    Graph c;

    a.CreateNode();
    b.CreateNode();

    std::vector<std::string> strA = {"a"};
    std::vector<std::string> strB = {"a"};
    std::vector<std::string> strC = {};

    ExpectMatchStr(a, b, strA, strB, 0, {{0, 0}});
    ExpectMatchStr(a, c, strA, strC, kSkipCost, {});
    ExpectMatchStr(c, a, strC, strA, kSkipCost, {});
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

    std::vector<std::string> strA = {"a", "b", "c", "d"};
    std::vector<std::string> strB = {"a", "b", "c", "d"};

    ExpectMatchStr(
        a, b, strA, strB, 0,
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

    std::vector<std::string> strA = {"a", "b", "c", "c", "d", "c"};
    std::vector<std::string> strB = {"a", "b", "c", "c", "d", "c"};

    ExpectMatchStr(
        a, b, strA, strB, 0,
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

    std::vector<std::string> strA = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k"};
    std::vector<std::string> strB = {"a", "b", "c", "e", "f", "g", "h", "h", "i", "j", "k"};

    ExpectMatchStr(
        a, b, strA, strB, 2 * kSkipCost,
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
    std::vector<std::string> strA = {"a", "b", "c", "c", "d", "d", "e", "e", "f", "f", "g"};
    std::vector<std::string> strB = {"a", "b", "c", "d", "e", "f", "g"};

    ExpectMatchStr(
        a, b, strA, strB, 4 * kSkipCost,
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

    std::vector<std::string> strA = {"a", "b", "c", "d"};

    for (size_t i = 0; i < 12498; ++i) {
        strA.push_back("e");
        strA.push_back("f");
        strA.push_back("g");
        strA.push_back("h");
    }
    strA.push_back("a");
    strA.push_back("b");
    strA.push_back("c");
    strA.push_back("d");
    strA.push_back("e");

    std::vector<std::string> strB = {"a", "b", "c", "d"};
    for (size_t i = 0; i < 9998; ++i) {
        strB.push_back("e");
        strB.push_back("f");
        strB.push_back("g");
        strB.push_back("h");
    }
    strB.push_back("h");
    strB.push_back("i");
    strB.push_back("j");
    strB.push_back("k");
    strB.push_back("l");

    std::vector<NodePair> expected_nodes;
    for (size_t i = 0; i < 39996; ++i)
        expected_nodes.push_back(NodePair(i, i));

    ExpectMatchStr(
        a, b, strA, strB, 10 * kSkipCost, expected_nodes);
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

    std::vector<std::string> strA = {"a", "b", "c", "d"};

    for (size_t i = 0; i < 2499; ++i) {
        strA.push_back("e");
        strA.push_back("f");
        strA.push_back("g");
        strA.push_back("h");
    }
    for (size_t i = 0; i < 2499; ++i) {
        strA.push_back("w");
        strA.push_back("x");
        strA.push_back("y");
        strA.push_back("z");
    }
    strA.push_back("a");
    strA.push_back("b");
    strA.push_back("c");
    strA.push_back("d");
    strA.push_back("e");

    std::vector<std::string> strB = {"a", "b", "c", "d"};
    for (size_t i = 0; i < 4999; ++i) {
        strB.push_back("e");
        strB.push_back("f");
        strB.push_back("g");
        strB.push_back("h");
    }
    for (size_t i = 0; i < 4999; ++i) {
        strB.push_back("w");
        strB.push_back("x");
        strB.push_back("y");
        strB.push_back("z");
    }
    strB.push_back("h");
    strB.push_back("i");
    strB.push_back("j");
    strB.push_back("k");
    strB.push_back("l");

    std::vector<NodePair> expected_nodes;
    for (size_t i = 0; i < 4; ++i)
        expected_nodes.push_back(NodePair(i, i));
    for (size_t i = 4; i < 10000; ++i)
        expected_nodes.push_back(NodePair(i, i));
    for (size_t i = 10000; i < 19996; ++i)
        expected_nodes.push_back(NodePair(i, i + 10000));

    ExpectMatchStr(
        a, b, strA, strB, 10 * kSkipCost, expected_nodes);
}

TEST(HierarchyMatcher, File1) {
    Graph a;
    Graph b;

    Node& root_a = a.CreateNode();
    Node& root_b = b.CreateNode();

    std::vector<std::string> strA = {"root"};
    std::vector<std::string> strB = {"root"};

    std::string line;

    std::ifstream in_a("test_data/seq_left_1.txt");
    while (std::getline(in_a, line)) {
        auto& new_node = a.CreateNode();
        root_a.AddChild(new_node.id());
        strA.push_back(line);
    }
    in_a.close();

    std::ifstream in_b("test_data/seq_right_1.txt");
    while (std::getline(in_b, line)) {
        auto& new_node = b.CreateNode();
        root_b.AddChild(new_node.id());
        strB.push_back(line);
    }
    in_b.close();

    std::vector<NodePair> expected_nodes;
    std::ifstream in_match("test_data/seq_match_1.txt");

    size_t first_id = 0;
    while (in_match >> first_id)
    {
        size_t second_id = 0;
        in_match >> second_id;
        expected_nodes.push_back(NodePair(first_id, second_id));
    }

    ExpectMatchStr(
        a, b, strA, strB, 12009 * kSkipCost, expected_nodes);
}

}    // namespace execution
}    // namespace tibee
