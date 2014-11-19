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
#include "execution/DynamicMatching.hpp"
#include "execution/Graph.hpp"

namespace tibee {
namespace execution {

namespace
{

const size_t kSkipCost = 2;

void ExpectMatch(const UIDSequence& uids_a, const UIDSequence& uids_b,
                 const MatchVector& expected_match, uint64_t expected_cost)
{
    CanonicalSequence canonical_a;
    CanonicalSequence canonical_b;

    CanonizeSequence(uids_a, &canonical_a);
    CanonizeSequence(uids_b, &canonical_b);

    Graph a;
    Graph b;

    auto& root_a = a.CreateNode();
    auto& root_b = b.CreateNode();

    for (size_t i = 0; i < uids_a.size(); ++i)
    {
        auto& new_node = a.CreateNode();
        root_a.AddChild(new_node.id());
    }

    for (size_t i = 0; i < uids_b.size(); ++i)
    {
        auto& new_node = b.CreateNode();
        root_b.AddChild(new_node.id());
    }

    // Match the sequences.
    MatchVector match;
    uint64_t cost = DynamicMatching(
        canonical_a, canonical_b,
        uids_a, uids_b,
        root_a, root_b,
        0, canonical_a.size(),
        0, canonical_b.size(),
        kSkipCost, &match);

    // Sort the matches.
    MatchVector expected_match_sorted(expected_match.begin(), expected_match.end());

    std::sort(expected_match_sorted.begin(), expected_match_sorted.end());
    std::sort(match.begin(), match.end());

    // Check the result.
    EXPECT_EQ(expected_cost, cost);
    EXPECT_EQ(expected_match_sorted, match);
}

}  // namespace

TEST(DynamicMatching, Empty)
{
    UIDSequence a = {};
    UIDSequence b = {};

    ExpectMatch(a, b, {}, 0);
}

TEST(DynamicMatching, Identical)
{
    UIDSequence a = {"a", "b", "c"};
    UIDSequence b = {"a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}},
        0);
}

TEST(DynamicMatching, IdenticalWithRepetition)
{
    UIDSequence a = {"a", "b", "c", "b", "c", "b", "c", "d"};
    UIDSequence b = {"a", "b", "c", "b", "c", "b", "c", "d"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}},
        0);
}

TEST(DynamicMatching, IdenticalWithRepetitionAtEnd)
{
    UIDSequence a = {"a", "b", "c", "b", "c", "b", "c"};
    UIDSequence b = {"a", "b", "c", "b", "c", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
        0);
}

TEST(DynamicMatching, IdenticalWithRepetitionAtBeginning)
{
    UIDSequence a = {"b", "c", "b", "c", "b", "c", "a"};
    UIDSequence b = {"b", "c", "b", "c", "b", "c", "a"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
        0);
}

TEST(DynamicMatching, InsertionAtBeginning)
{
    UIDSequence a = {"a", "b", "c"};
    UIDSequence b = {"z", "a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 2}, {2, 3}, {3, 4}},
        1 * kSkipCost);
}

TEST(DynamicMatching, InsertionAtEnd)
{
    UIDSequence a = {"a", "b", "c"};
    UIDSequence b = {"a", "b", "c", "z"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}},
        1 * kSkipCost);
}

TEST(DynamicMatching, InsertionMiddle)
{
    UIDSequence a = {"a", "b", "c"};
    UIDSequence b = {"a", "b", "z", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 4}},
        1 * kSkipCost);
}

TEST(DynamicMatching, InsertionRepetitionMiddle)
{
    UIDSequence a = {"a", "b", "c"};
    UIDSequence b = {"a", "b", "z", "z", "z", "z", "z", "z", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 9}},
        1 * kSkipCost);
}

TEST(DynamicMatching, DeletionAtBeginning)
{
    UIDSequence a = {"z", "a", "b", "c"};
    UIDSequence b = {"a", "b", "c"};

    ExpectMatch(
        a, b,
        {{2, 1}, {3, 2}, {4, 3}},
        1 * kSkipCost);
}

TEST(DynamicMatching, DeletionAtEnd)
{
    UIDSequence a = {"a", "b", "c", "z"};
    UIDSequence b = {"a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {3, 3}},
        1 * kSkipCost);
}

TEST(DynamicMatching, DeletionMiddle)
{
    UIDSequence a = {"a", "b", "z", "c"};
    UIDSequence b = {"a", "b", "c"};

    ExpectMatch(
        a, b,
        {{1, 1}, {2, 2}, {4, 3}},
        1 * kSkipCost);
}

TEST(DynamicMatching, LongGap)
{
    UIDSequence a = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l"};
    UIDSequence b = {"b", "c", "l"};

    ExpectMatch(
        a, b,
        {{2, 1}, {3, 2}, {12, 3}},
        9 * kSkipCost);
}

TEST(DynamicMatching, TwoRepetitions)
{
    UIDSequence a = {"a", "b", "b", "b", "b", "c", "d", "d", "d", "d"};
    UIDSequence b = {"z", "a", "z", "b", "b", "b", "b", "z", "c", "d", "d", "d", "d", "z"};

    ExpectMatch(
        a, b,
        {{1, 2}, {2, 4}, {3, 5}, {4, 6}, {5, 7}, {6, 9}, {7, 10}, {8, 11}, {9, 12}, {10, 13}},
        4 * kSkipCost);
}

}    // namespace execution
}    // namespace tibee
