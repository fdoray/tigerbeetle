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
#include "execution/FindRepetitions.hpp"

namespace tibee {
namespace execution {

TEST(FindRepetitions, Single)
{
    std::vector<std::string> seq = {"a", "b", "c", "a", "b", "a", "b", "a", "b", "a", "b", "b", "e"};
    std::vector<Repetition> repetitions;
    FindRepetitions(seq, 2, &repetitions);

    std::vector<Repetition> expected_repetitions = {
        Repetition(3, 4)};

    EXPECT_EQ(expected_repetitions, repetitions);
}

TEST(FindRepetitions, Multiple)
{
    std::vector<std::string> seq = {"a", "b", "a", "b", "c", "d", "e", "f", "e", "f", "e", "f"};
    std::vector<Repetition> repetitions;
    FindRepetitions(seq, 2, &repetitions);

    std::vector<Repetition> expected_repetitions = {
        Repetition(0, 2),
        Repetition(6, 3)};

    EXPECT_EQ(expected_repetitions, repetitions);
}

TEST(FindRepetitions, Long)
{
    std::vector<std::string> seq = {"a", "b", "c", "d", "a", "b", "c", "d", "a", "b", "c", "d", "e"};
    std::vector<Repetition> repetitions;
    FindRepetitions(seq, 4, &repetitions);

    std::vector<Repetition> expected_repetitions = {
        Repetition(0, 3)};

    EXPECT_EQ(expected_repetitions, repetitions);
}

}    // namespace execution
}    // namespace tibee
