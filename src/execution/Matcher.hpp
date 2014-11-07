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
#ifndef _TIBEE_EXECUTION_MATCHER_HPP
#define _TIBEE_EXECUTION_MATCHER_HPP

#include <boost/functional.hpp>

#include "execution/NodePair.hpp"
#include "execution/Graph.hpp"

namespace tibee {
namespace execution {

// A huge cost.
uint64_t kHugeCost = -1;

typedef std::function<uint64_t (NodeId a,
                                NodeId b)> MatchNodesCostFunc;

typedef std::vector<NodePair> MatchVector;

uint64_t MatchGraphs(const Graph& graph_a,
                     const Graph& graph_b,
                     const MatchNodesCostFunc& match_cost_func,
                     uint64_t skip_cost,
                     MatchVector* matching_nodes);

}    // namespace execution
}    // namespace tibee

#endif    // _TIBEE_EXECUTION_MATCHER_HPP