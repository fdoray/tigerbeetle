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
#ifndef _TIBEE_ANALYSIS_TIMELINEGRAPH_NODE_HPP
#define _TIBEE_ANALYSIS_TIMELINEGRAPH_NODE_HPP

#include <assert.h> 
#include <stddef.h>

namespace tibee {
namespace analysis {
namespace timeline_graph {

typedef size_t NodeId;
const NodeId kInvalidNodeId = -1;

class Node {
public:
    enum ChildIndex {
        kChildVertical = 0,
        kChildHorizontal,
        kChildCount
    };

    Node(NodeId id);
    ~Node();

    NodeId id() const { return id_; }

    NodeId horizontal_child() const {
        return children_[kChildHorizontal];
    }
    void set_horizontal_child(NodeId horizontal_child) {
        children_[kChildHorizontal] = horizontal_child;
    }

    NodeId vertical_child() const { return children_[kChildVertical]; }
    void set_vertical_child(NodeId vertical_child) {
        children_[kChildVertical] = vertical_child;
    }

    NodeId child(ChildIndex index) const { return children_[index]; }
    void set_child(ChildIndex index, NodeId child) {
        assert(index < kChildCount);
        children_[index] = child;
    }

private:
    // Identifier of the timeline node.
    NodeId id_;

    // Identifiers of the child nodes.
    NodeId children_[kChildCount];
};

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee

#endif    // _TIBEE_ANALYSIS_TIMELINEGRAPH_NODE_HPP
