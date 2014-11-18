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
#include "execution/FindRepetitions.hpp"

#include <algorithm>
#include <iostream>

namespace tibee {
namespace execution {

namespace
{

bool SubsequencesEqual(
    const std::vector<std::string>& seq,
    size_t index_a,
    size_t index_b,
    size_t size)
{
    return std::equal(seq.begin() + index_a,
                      seq.begin() + index_a + size,
                      seq.begin() + index_b);
}

}  // namespace

void FindRepetitions(
    const std::vector<std::string>& seq,
    size_t chunk_size,
    std::vector<Repetition>* repetitions)
{
    if (chunk_size == 0 || seq.size() < chunk_size * 2)
        return;

    for (size_t i = chunk_size; i <= seq.size() - chunk_size; ++i)
    {
        if (!SubsequencesEqual(seq, i - chunk_size, i, chunk_size))
            continue;

        // This chunk is equal with the previous chunk!
        // Try to find more chunks.
        size_t num_repetitions = 2;

        for (size_t j = i + chunk_size; j <= seq.size() - chunk_size; j += chunk_size)
        {
            if (!SubsequencesEqual(seq, i, j, chunk_size))
                break;
            ++num_repetitions;
        }

        // Excellent! We found many repetitions.
        repetitions->push_back(Repetition(i - chunk_size, num_repetitions, chunk_size));

        // Skip to the end of the repetitions.
        i = (i - chunk_size) + num_repetitions * chunk_size - 1;
    }
}

void FindRepetitionsMap(
    const std::vector<std::string>& seq,
    size_t chunk_size,
    RepetitionsMap* repetitions)
{
    std::vector<Repetition> repetitions_vector;
    FindRepetitions(seq, chunk_size, &repetitions_vector);

    for (const auto& repetition : repetitions_vector)
        (*repetitions)[repetition.start_index] = repetition.num_repetitions;
}

}  // namespace execution
}  // namespace tibee
