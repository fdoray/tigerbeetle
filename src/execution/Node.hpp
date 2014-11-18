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
#ifndef _TIBEE_EXECUTION_NODE_HPP
#define _TIBEE_EXECUTION_NODE_HPP

#include <boost/noncopyable.hpp>
#include <stddef.h>
#include <vector>

namespace tibee {
namespace execution {

typedef size_t NodeId;
const NodeId kInvalidNodeId = -1;

/**
 * Execution graph node.
 *
 * @author Francois Doray
 */
class Node
    : public boost::noncopyable
{
public:
    typedef std::vector<NodeId> ChildrenVector;
    typedef ChildrenVector::const_iterator ChildrenIterator;

    Node(NodeId id);
    ~Node();

    NodeId id() const { return id_; }

    void AddChild(NodeId child_id);

    size_t NumChildren() const { return children_.size(); }
    NodeId GetChild(size_t index) const { return children_[index]; }

    ChildrenIterator begin() const { return children_.begin(); }
    ChildrenIterator end() const { return children_.end(); }

private:
    // Identifier of this node.
    NodeId id_;

    // Identifiers of the child nodes.
    ChildrenVector children_;
};

}    // namespace execution
}    // namespace tibee

#endif    // _TIBEE_EXECUTION_NODE_HPP
