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
#include "execution/CanonizeSequence.hpp"

namespace tibee {
namespace execution {

TEST(CanonizeSequence, Simple)
{
    UIDSequence uid_seq = {"a", "b", "c", "b", "c", "b", "c", "d"};
    CanonicalSequence canonical_seq;

    CanonizeSequence(uid_seq, &canonical_seq);

    CanonicalSequence expected_canonical_seq = {
        {0, 0, 0}, {1, 2, 3}, {7, 0, 0}};

    EXPECT_EQ(expected_canonical_seq, canonical_seq);
}

TEST(CanonizeSequence, ManyRepetitions)
{
    UIDSequence uid_seq = {"a", "b", "c", "b", "c", "d", "b", "c", "b", "c", "d", "e"};
    CanonicalSequence canonical_seq;

    CanonizeSequence(uid_seq, &canonical_seq);

    CanonicalSequence expected_canonical_seq = {
        {0, 0, 0}, {1, 5, 2}, {11, 0, 0}};

    EXPECT_EQ(expected_canonical_seq, canonical_seq);
}

TEST(CanonizeSequence, NoRepetitions)
{
    UIDSequence uid_seq = {"a", "b", "c", "d", "e", "f"};
    CanonicalSequence canonical_seq;

    CanonizeSequence(uid_seq, &canonical_seq);

    CanonicalSequence expected_canonical_seq = {
        {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0}, {4, 0, 0}, {5, 0, 0}};

    EXPECT_EQ(expected_canonical_seq, canonical_seq);
}

TEST(CanonizeSequence, RepetitionBeginning)
{
    UIDSequence uid_seq = {"a", "b", "a", "b", "c"};
    CanonicalSequence canonical_seq;

    CanonizeSequence(uid_seq, &canonical_seq);

    CanonicalSequence expected_canonical_seq = {
        {0, 2, 2}, {4, 0, 0}};

    EXPECT_EQ(expected_canonical_seq, canonical_seq);
}

TEST(CanonizeSequence, RepetitionEnd)
{
    UIDSequence uid_seq = {"c", "a", "b", "a", "b"};
    CanonicalSequence canonical_seq;

    CanonizeSequence(uid_seq, &canonical_seq);

    CanonicalSequence expected_canonical_seq = {
        {0, 0, 0}, {1, 2, 2}};

    EXPECT_EQ(expected_canonical_seq, canonical_seq);
}

}    // namespace execution
}    // namespace tibee
