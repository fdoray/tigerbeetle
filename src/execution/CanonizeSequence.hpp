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
#ifndef _TIBEE_EXECUTION_CANONIZESEQUENCE_HPP
#define _TIBEE_EXECUTION_CANONIZESEQUENCE_HPP

#include <vector>

#include "execution/MatcherCommon.hpp"
#include "execution/Node.hpp"

namespace tibee {
namespace execution {

struct CanonicalNode {
    CanonicalNode() {}
    CanonicalNode(size_t pos, size_t chunk_size, size_t num_repetitions)
        : pos(pos), chunk_size(chunk_size), num_repetitions(num_repetitions)
    {}

    size_t pos;
    size_t chunk_size;
    size_t num_repetitions;

    bool operator==(const CanonicalNode& other) const {
        return pos == other.pos &&
            chunk_size == other.chunk_size &&
            num_repetitions == other.num_repetitions;
    }
};

typedef std::vector<CanonicalNode> CanonicalSequence;

void CanonizeSequence(
    const UIDSequence& sequence,
    CanonicalSequence* canonical_sequence);

}  // namespace execution
}  // namespace tibee

#endif    // _TIBEE_EXECUTION_CANONIZESEQUENCE_HPP
