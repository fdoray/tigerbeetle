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
#include "analysis/execution_graph/Graph.hpp"

#include <assert.h>

namespace tibee {
namespace analysis {
namespace execution_graph {

Graph::Graph() {
}

Graph::~Graph() {
}

Node& Graph::CreateNode() {
    nodes_.push_back(
        std::unique_ptr<Node>(new Node(nodes_.size())));
    return *nodes_.back();
}
    
const Node& Graph::GetNode(NodeId node_id) const
{
    assert(node_id < nodes_.size());
    return *nodes_[node_id];
}

Node& Graph::GetNode(NodeId node_id)
{
    assert(node_id < nodes_.size());
    return *nodes_[node_id];
}

PreorderIterator Graph::preorder_begin() const
{
    if (nodes_.empty())
        return preorder_end();
    return PreorderIterator(this, 0);
}

PreorderIterator Graph::preorder_end() const
{
    return PreorderIterator(this);
}

}    // namespace execution_graph
}    // namespace analysis
}    // namespace tibee
