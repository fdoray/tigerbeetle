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
#include "execution/CanonizeSequence.hpp"
#include "execution/HierarchyMatcher.hpp"
#include "execution/Graph.hpp"

namespace tibee {
namespace execution {

namespace
{

const size_t kSkipCost = 2;

UID UniqueIdentifier(
    const UIDSequence& uids_a,
    const UIDSequence& uids_b,
    NodeId node_id,
    GraphPosition position)
{
    if (position == GraphPosition::LEFT_GRAPH)
      return uids_a[node_id];
    return uids_b[node_id];
}

void ExpectMatch(const UIDSequence& uids_a, const UIDSequence& uids_b,
                 const MatchVector& expected_match, uint64_t expected_cost)
{
    namespace pl = std::placeholders;

    Graph a;
    Graph b;

    auto& root_a = a.CreateNode();
    auto& root_b = b.CreateNode();

    for (size_t i = 1; i < uids_a.size(); ++i)
    {
        auto& new_node = a.CreateNode();
        root_a.AddChild(new_node.id());
    }

    for (size_t i = 1; i < uids_b.size(); ++i)
    {
        auto& new_node = b.CreateNode();
        root_b.AddChild(new_node.id());
    }

    // Match the sequences.
    MatchVector match;
    uint64_t cost = MatchGraphsHierarchical(
        a, b,
        std::bind(UniqueIdentifier,
                  std::ref(uids_a),
                  std::ref(uids_b),
                  pl::_1,
                  pl::_2),
        kSkipCost,
        &match);

    // Sort the matches.
    MatchVector expected_match_sorted(expected_match.begin(), expected_match.end());

    std::sort(expected_match_sorted.begin(), expected_match_sorted.end());
    std::sort(match.begin(), match.end());

    // Check the result.
    EXPECT_EQ(expected_cost, cost);
    EXPECT_EQ(expected_match_sorted, match);
}

}  // namespace

TEST(HierarchyMatching, Empty)
{
    UIDSequence a = {"*"};
    UIDSequence b = {"*"};

    ExpectMatch(a, b, {}, 0);
}

TEST(HierarchyMatching, Identical)
{
    UIDSequence a = {"*", "a", "b", "c"};
    UIDSequence b = {"*", "a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}},
        0);
}

TEST(HierarchyMatching, IdenticalWithRepetition)
{
    UIDSequence a = {"*", "a", "b", "c", "b", "c", "b", "c", "d"};
    UIDSequence b = {"*", "a", "b", "c", "b", "c", "b", "c", "d"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}},
        0);
}

TEST(HierarchyMatching, IdenticalWithRepetitionAtEnd)
{
    UIDSequence a = {"*", "a", "b", "c", "b", "c", "b", "c"};
    UIDSequence b = {"*", "a", "b", "c", "b", "c", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
        0);
}

TEST(HierarchyMatching, IdenticalWithRepetitionAtBeginning)
{
    UIDSequence a = {"*", "b", "c", "b", "c", "b", "c", "a"};
    UIDSequence b = {"*", "b", "c", "b", "c", "b", "c", "a"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
        0);
}

TEST(HierarchyMatching, InsertionAtBeginning)
{
    UIDSequence a = {"*", "a", "b", "c"};
    UIDSequence b = {"*", "z", "a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 2}, {2, 3}, {3, 4}},
        1 * kSkipCost);
}

TEST(HierarchyMatching, InsertionAtEnd)
{
    UIDSequence a = {"*", "a", "b", "c"};
    UIDSequence b = {"*", "a", "b", "c", "z"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}},
        1 * kSkipCost);
}

TEST(HierarchyMatching, InsertionMiddle)
{
    UIDSequence a = {"*", "a", "b", "c"};
    UIDSequence b = {"*", "a", "b", "z", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 4}},
        1 * kSkipCost);
}

TEST(HierarchyMatching, InsertionRepetitionMiddle)
{
    UIDSequence a = {"*", "a", "b", "c"};
    UIDSequence b = {"*", "a", "b", "z", "z", "z", "z", "z", "z", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 9}},
        1 * kSkipCost);
}

TEST(HierarchyMatching, DeletionAtBeginning)
{
    UIDSequence a = {"*", "z", "a", "b", "c"};
    UIDSequence b = {"*", "a", "b", "c"};

    ExpectMatch(
        a, b,
        {{2, 1}, {3, 2}, {4, 3}},
        1 * kSkipCost);
}

TEST(HierarchyMatching, DeletionAtEnd)
{
    UIDSequence a = {"*", "a", "b", "c", "z"};
    UIDSequence b = {"*", "a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}},
        1 * kSkipCost);
}

TEST(HierarchyMatching, DeletionMiddle)
{
    UIDSequence a = {"*", "a", "b", "z", "c"};
    UIDSequence b = {"*", "a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {4, 3}},
        1 * kSkipCost);
}

TEST(HierarchyMatching, LongGap)
{
    UIDSequence a = {"*", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l"};
    UIDSequence b = {"*", "b", "c", "l"};

    ExpectMatch(
        a, b,
        {{2, 1}, {3, 2}, {12, 3}},
        9 * kSkipCost);
}

TEST(HierarchyMatching, TwoRepetitions)
{
    UIDSequence a = {"*", "a", "b", "b", "b", "b", "c", "d", "d", "d", "d"};
    UIDSequence b = {"*", "z", "a", "z", "b", "b", "b", "b", "z", "c", "d", "d", "d", "d", "z"};

    ExpectMatch(
        a, b,
        {{1, 2}, {2, 4}, {3, 5}, {4, 6}, {5, 7}, {6, 9}, {7, 10}, {8, 11}, {9, 12}, {10, 13}},
        4 * kSkipCost);
}

}    // namespace execution
}    // namespace tibee
