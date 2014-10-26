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
#include "analysis/execution_graph/Node.hpp"

#include "base/print.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

using base::tbendl;
using base::tberror;

Node::Node(NodeId id)
    : id_(id)
{
}

Node::~Node()
{
}

void Node::AddChild(NodeId child_id)
{
    if (child_id == id()) {
        tberror() << "Cannot add a node as its own child." << tbendl();
        return;
    }
    children_.push_back(child_id);
}

}    // namespace execution_graph
}    // namespace analysis
}    // namespace tibee