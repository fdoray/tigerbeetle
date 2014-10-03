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
#include "analysis/timeline_graph/Matcher.hpp"
#include "analysis/timeline_graph/TimelineGraph.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

namespace {

namespace pl = std::placeholders;

uint64_t kHugeCost = -1;

uint64_t SimpleMatchCostFunc(
    const TimelineGraph& graph_a,
    const TimelineGraph& graph_b,
    uint64_t match_diff_structure_cost,
    NodeId a,
    NodeId b)
{
    // Don't match a branching node with a non-branching node.
    NodeId vertical_child_a = graph_a.GetNode(a).vertical_child();
    NodeId vertical_child_b = graph_b.GetNode(b).vertical_child();

    if ((vertical_child_a == kInvalidNodeId &&
         vertical_child_b != kInvalidNodeId) ||
        (vertical_child_a != kInvalidNodeId &&
         vertical_child_b == kInvalidNodeId))
    {
        return match_diff_structure_cost;
    }

    return 0;
}

uint64_t CharMatchCostFunc(
    const TimelineGraph& graph_a,
    const TimelineGraph& graph_b,
    const std::vector<char>& char_a,
    const std::vector<char>& char_b,
    uint64_t match_diff_structure_cost,
    NodeId a,
    NodeId b)
{
    return std::max(
            SimpleMatchCostFunc(graph_a, graph_b, match_diff_structure_cost, a, b),
            static_cast<uint64_t>(char_a[a] == char_b[b] ? 0 : 2));
}

void ExpectMatchSimple(const TimelineGraph& a,
                       const TimelineGraph& b,
                       uint64_t match_diff_structure_cost,
                       uint64_t expected_cost,
                       const std::vector<NodePair>& expected_nodes)
{
    // Match |graph_a| with |graph_b|.
    std::vector<NodePair> actual_nodes;
    EXPECT_EQ(expected_cost, MatchGraphs(
        a, b,
        std::bind(SimpleMatchCostFunc,
                  std::ref(a),
                  std::ref(b),
                  match_diff_structure_cost,
                  pl::_1,
                  pl::_2),
        1,
        &actual_nodes));

    EXPECT_EQ(expected_nodes, actual_nodes);

    // Match |graph_b| with |graph_a|.
    std::vector<NodePair> actual_nodes_reverse;
    EXPECT_EQ(expected_cost, MatchGraphs(
        b, a,
        std::bind(SimpleMatchCostFunc,
                  std::ref(b),
                  std::ref(a),
                  match_diff_structure_cost,
                  pl::_1,
                  pl::_2),
        1,
        &actual_nodes_reverse));

    std::vector<NodePair> expected_nodes_reverse;
    for (const NodePair& pair : expected_nodes) {
        expected_nodes_reverse.push_back(
                NodePair(pair.node_id_b(), pair.node_id_a()));
    }

    EXPECT_EQ(expected_nodes_reverse, actual_nodes_reverse);
}

void ExpectMatchChar(const TimelineGraph& a,
                     const TimelineGraph& b,
                     const std::vector<char>& char_a,
                     const std::vector<char>& char_b,
                     uint64_t match_diff_structure_cost,
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
                  match_diff_structure_cost,
                  pl::_1,
                  pl::_2),
        1,
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
                  match_diff_structure_cost,
                  pl::_1,
                  pl::_2),
        1,
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
    TimelineGraph a;
    TimelineGraph b;

    ExpectMatchSimple(a, b, kHugeCost, 0, {});
}

TEST(Matcher, OneNodeGraphs) {
    TimelineGraph a;
    TimelineGraph b;
    TimelineGraph c;

    a.CreateNode();
    b.CreateNode();

    ExpectMatchSimple(a, b, kHugeCost, 0, {{0, 0}});
    ExpectMatchSimple(b, c, kHugeCost, 1, {});
}

TEST(Matcher, IdenticalGraphs) {
    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 8; ++i) {
        a.CreateNode();
        b.CreateNode();
    }

    a.GetNode(0).set_horizontal_child(1);
    a.GetNode(1).set_horizontal_child(4);
    a.GetNode(1).set_vertical_child(2);
    a.GetNode(2).set_horizontal_child(3);
    a.GetNode(4).set_horizontal_child(7);
    a.GetNode(4).set_vertical_child(5);
    a.GetNode(5).set_horizontal_child(6);

    b.GetNode(0).set_horizontal_child(1);
    b.GetNode(1).set_horizontal_child(2);
    b.GetNode(1).set_vertical_child(4);
    b.GetNode(4).set_horizontal_child(5);
    b.GetNode(2).set_horizontal_child(3);
    b.GetNode(2).set_vertical_child(6);
    b.GetNode(6).set_horizontal_child(7);

    ExpectMatchSimple(
        a, b, kHugeCost, 0,
        {{0, 0}, {1, 1}, {2, 4}, {3, 5}, {4, 2}, {5, 6}, {6, 7}, {7, 3}});
}

TEST(Matcher, BranchInsertion) {
    // Graph A:
    //            0 - 1 - 2
    //
    // Graph B:
    //            0 - 1 - 2
    //                    |
    //                    3

    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 3; ++i) {
        a.CreateNode();
        b.CreateNode();
    }

    a.GetNode(0).set_horizontal_child(1);
    a.GetNode(1).set_horizontal_child(2);

    b.CreateNode();
    b.GetNode(0).set_horizontal_child(1);
    b.GetNode(1).set_horizontal_child(2);
    b.GetNode(1).set_vertical_child(3);

    ExpectMatchSimple(a, b, 0, 1, {{0, 0}, {1, 1}, {2, 2}});
    ExpectMatchSimple(a, b, 1, 2, {{0, 0}, {1, 1}, {2, 2}});
}

TEST(Matcher, NodeInsertion) {
    // Graph A:
    //            0 - 1 - 2 - 3 - 4
    //                    |
    //                    5 - 6 - 7
    //
    // Graph B:
    //            0 - 1 - 2 - 3 - 4 - 8
    //                            |
    //                            5 - 6 - 7

    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 8; ++i)
        a.CreateNode();

    a.GetNode(0).set_horizontal_child(1);
    a.GetNode(1).set_horizontal_child(2);
    a.GetNode(2).set_horizontal_child(3);
    a.GetNode(3).set_horizontal_child(4);
    a.GetNode(1).set_vertical_child(5);
    a.GetNode(5).set_horizontal_child(6);
    a.GetNode(6).set_horizontal_child(7);

    for (size_t i = 0; i < 9; ++i)
        b.CreateNode();

    b.GetNode(0).set_horizontal_child(1);
    b.GetNode(1).set_horizontal_child(2);
    b.GetNode(2).set_horizontal_child(3);
    b.GetNode(3).set_horizontal_child(4);
    b.GetNode(4).set_horizontal_child(8);
    b.GetNode(2).set_vertical_child(5);
    b.GetNode(5).set_horizontal_child(6);
    b.GetNode(6).set_horizontal_child(7);

    ExpectMatchSimple(
        a, b, kHugeCost, 1,
        {{0, 0}, {1, 2}, {5, 5}, {6, 6}, {7, 7}, {2, 3}, {3, 4}, {4, 8}});

}

TEST(Matcher, EditDistanceA) {
    // Graph A:
    //        0=A - 1=B - 2=A - 3=B - 4=C
    //
    // Graph B:
    //        0=A - 1=B - 2=C

    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 5; ++i)
        a.CreateNode();

    a.GetNode(0).set_horizontal_child(1);
    a.GetNode(1).set_horizontal_child(2);
    a.GetNode(2).set_horizontal_child(3);
    a.GetNode(3).set_horizontal_child(4);

    for (size_t i = 0; i < 3; ++i)
        b.CreateNode();

    b.GetNode(0).set_horizontal_child(1);
    b.GetNode(1).set_horizontal_child(2);

    ExpectMatchChar(
        a, b,
        {'A', 'B', 'A', 'B', 'Z'},
        {'A', 'B', 'Z'},
        kHugeCost, 2,
        {{0, 0}, {1, 1}, {4, 2}});
}

TEST(Matcher, EditDistanceB) {
    // Graph A:
    //        0=A - 1=A - 2=A - 3=C - 4=C
    //
    // Graph B:
    //        0=D - 1=A - 2=A - 3=A

    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 5; ++i)
        a.CreateNode();

    a.GetNode(0).set_horizontal_child(1);
    a.GetNode(1).set_horizontal_child(2);
    a.GetNode(2).set_horizontal_child(3);
    a.GetNode(3).set_horizontal_child(4);

    for (size_t i = 0; i < 4; ++i)
        b.CreateNode();

    b.GetNode(0).set_horizontal_child(1);
    b.GetNode(1).set_horizontal_child(2);
    b.GetNode(2).set_horizontal_child(3);

    ExpectMatchChar(
        a, b,
        {'A', 'A', 'A', 'C', 'C'},
        {'D', 'A', 'A', 'A'},
        kHugeCost, 3,
        {{0, 1}, {1, 2}, {2, 3}});
}

TEST(Matcher, EditDistanceC) {
    // Graph A:
    //            0=A 
    //             |
    //            1=A
    //             |
    //            2=B
    //
    // Graph B:
    //            0=A 
    //             |
    //            1=A

    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 3; ++i)
        a.CreateNode();

    a.GetNode(0).set_vertical_child(1);
    a.GetNode(1).set_vertical_child(2);

    for (size_t i = 0; i < 2; ++i)
        b.CreateNode();

    b.GetNode(0).set_vertical_child(1);

    ExpectMatchChar(
        a, b,
        {'A', 'B', 'A'},
        {'A', 'A'},
        kHugeCost, 3,
        {{0, 0}});

    ExpectMatchChar(
        a, b,
        {'A', 'A', 'B'},
        {'A', 'A'},
        0, 1,
        {{0, 0}, {1, 1}});
}

TEST(Matcher, EditDistanceWithBranch) {
  // Graph A:
  //      0=A - 1=B - 2=A - 3=B - 4=Z
  //                         |
  //                        5=F
  //
  // Graph B:
  //      0=A - 1=B - 2=Z
  //             |
  //            3=F

    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 6; ++i)
        a.CreateNode();

    a.GetNode(0).set_horizontal_child(1);
    a.GetNode(1).set_horizontal_child(2);
    a.GetNode(2).set_horizontal_child(3);
    a.GetNode(3).set_horizontal_child(4);
    a.GetNode(3).set_vertical_child(5);

    for (size_t i = 0; i < 4; ++i)
        b.CreateNode();

    b.GetNode(0).set_horizontal_child(1);
    b.GetNode(1).set_horizontal_child(2);
    b.GetNode(1).set_vertical_child(3);

    ExpectMatchChar(
        a, b,
        {'A', 'B', 'A', 'B', 'Z', 'F'},
        {'A', 'B', 'Z', 'F'},
        kHugeCost, 2,
        {{0, 0}, {3, 1}, {5, 3}, {4, 2}});
}

TEST(Matcher, EditDistanceWithBranches) {
  // Graph A:
  //      0=A - 1=B - 2=A - 3=B - 4=Z
  //                         |
  //                        5=F - 6=G
  //                               |
  //                              7=Z
  //
  // Graph B:
  //      0=A - 1=B - 2=Z
  //             |
  //            3=G
  //             |
  //            4=Z

    TimelineGraph a;
    TimelineGraph b;

    for (size_t i = 0; i < 8; ++i)
        a.CreateNode();

    a.GetNode(0).set_horizontal_child(1);
    a.GetNode(1).set_horizontal_child(2);
    a.GetNode(2).set_horizontal_child(3);
    a.GetNode(3).set_horizontal_child(4);
    a.GetNode(3).set_vertical_child(5);
    a.GetNode(5).set_horizontal_child(6);
    a.GetNode(6).set_vertical_child(7);

    for (size_t i = 0; i < 5; ++i)
        b.CreateNode();

    b.GetNode(0).set_horizontal_child(1);
    b.GetNode(1).set_horizontal_child(2);
    b.GetNode(1).set_vertical_child(3);
    b.GetNode(3).set_vertical_child(4);

    ExpectMatchChar(
        a, b,
        {'A', 'B', 'A', 'B', 'Z', 'F', 'G', 'Z'},
        {'A', 'B', 'Z', 'G', 'Z'},
        kHugeCost, 3,
        {{0, 0}, {3, 1}, {6, 3}, {7, 4}, {4, 2}});
}

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee
