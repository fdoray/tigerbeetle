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
#include "execution/CanonizeSequence.hpp"
#include "execution/FindRepetitions.hpp"

namespace tibee {
namespace execution {

namespace
{
const size_t kMinChunkSize = 2;
const size_t kMaxChunkSize = 8;
}

void CanonizeSequence(
    const UIDSequence& sequence,
    CanonicalSequence* canonical_sequence)
{
    // Find repetitions of 2 - 8 elements.
    std::vector<RepetitionsMap> repetitions(kMaxChunkSize + 1);
    for (size_t chunk_size = kMinChunkSize; chunk_size <= kMaxChunkSize; ++chunk_size)
        FindRepetitionsMap(sequence, chunk_size, &repetitions[chunk_size]);

    // Canonize the sequence.
    for (size_t i = 0; i < sequence.size();)
    {
        // Find the longest repetition at this position.
        size_t longest_repetition_chunk_size = 0;
        size_t longest_repetition_length = 1;
        size_t longest_repetition_num_repetitions = 0;
        for (size_t chunk_size = kMinChunkSize; chunk_size <= kMaxChunkSize; ++chunk_size)
        {
            auto look = repetitions[chunk_size].find(i);
            if (look != repetitions[chunk_size].end())
            {
                size_t repetition_length = chunk_size * look->second;
                if (repetition_length > longest_repetition_length)
                {
                    longest_repetition_length = repetition_length;
                    longest_repetition_chunk_size = chunk_size;
                    longest_repetition_num_repetitions = look->second;
                }
            }
        }

        // Output the canonical node.
        CanonicalNode node;
        node.pos = i;
        node.chunk_size = longest_repetition_chunk_size;
        node.num_repetitions = longest_repetition_num_repetitions;
        canonical_sequence->push_back(node);

        // Move to the next node.
        i += longest_repetition_length;
    }
}

}  // namespace execution
}  // namespace tibee
