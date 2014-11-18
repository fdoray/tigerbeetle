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
#ifndef _TIBEE_EXECUTION_FINDREPETITIONS_HPP
#define _TIBEE_EXECUTION_FINDREPETITIONS_HPP

#include <stddef.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace tibee {
namespace execution {

typedef std::unordered_map<size_t, size_t> RepetitionsMap;

struct Repetition {
    Repetition(size_t start_index, size_t num_repetitions)
        : start_index(start_index), num_repetitions(num_repetitions) {}
    size_t start_index;
    size_t num_repetitions;
    bool operator==(const Repetition& other) const {
        return start_index == other.start_index && num_repetitions == other.num_repetitions;
    }
};

void FindRepetitions(
    const std::vector<std::string>& seq,
    size_t chunk_size,
    std::vector<Repetition>* repetitions);

void FindRepetitionsMap(
    const std::vector<std::string>& seq,
    size_t chunk_size,
    RepetitionsMap* repetitions);

}  // namespace execution
}  // namespace tibee

#endif    // _TIBEE_EXECUTION_FINDREPETITIONS_HPP
