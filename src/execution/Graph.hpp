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
#ifndef _TIBEE_EXECUTION_GRAPH_HPP
#define _TIBEE_EXECUTION_GRAPH_HPP

#include <boost/noncopyable.hpp>
#include <memory>
#include <vector>

#include "execution/Node.hpp"
#include "execution/PreorderIterator.hpp"

namespace tibee {
namespace execution {

/**
 * Execution graph.
 *
 * @author Francois Doray
 */
class Graph
    : public boost::noncopyable
{
 public:
    typedef std::unique_ptr<Graph> UP;

    Graph();
    ~Graph();

    Node& CreateNode();
    
    const Node& GetNode(NodeId node_id) const;
    Node& GetNode(NodeId node_id);

    size_t size() const { return nodes_.size(); }
    bool empty() const { return size() == 0; }

    PreorderIterator preorder_begin() const;
    PreorderIterator preorder_end() const;

 private:
    // Nodes of the graph.
    typedef std::vector<std::unique_ptr<Node> > NodeVector;
    NodeVector nodes_;
};

}    // namespace execution
}    // namespace tibee

#endif    // _TIBEE_EXECUTION_GRAPH_HPP
