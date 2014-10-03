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
#include "analysis/timeline_graph/Node.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

Node::Node(NodeId id)
    : id_(id)
{
    for (size_t i = 0; i < kChildCount; ++i)
        children_[i] = kInvalidNodeId;
}

Node::~Node()
{
}


}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee