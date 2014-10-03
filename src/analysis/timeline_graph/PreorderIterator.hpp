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
#ifndef _TIBEE_ANALYSIS_TIMELINEGRAPH_PREORDERITERATOR_HPP
#define _TIBEE_ANALYSIS_TIMELINEGRAPH_PREORDERITERATOR_HPP

#include <iterator>
#include <vector>

#include "analysis/timeline_graph/Node.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

class TimelineGraph;

// Iterator that visits the nodes of a TimelineGraph in preorder.
// The ancestors of a node are always visited before the node itself.
class PreorderIterator :
    public std::iterator<std::forward_iterator_tag, Node> {
 public:
    // Constructs an iterator that points after the last node of the traversal.
    PreorderIterator(const TimelineGraph* graph);

    // Constructs an iterator that points the specified node.
    PreorderIterator(const TimelineGraph* graph,
                     NodeId start_node);

    ~PreorderIterator();

    PreorderIterator& operator++();
    bool operator==(const PreorderIterator& rhs) const;
    bool operator!=(const PreorderIterator& rhs) const;

    const Node& operator*();
    const Node* operator->();

    size_t Depth() const;

 private:
    bool MoveToNextChild();

    struct NodeInfo {
        NodeInfo(NodeId node_id, size_t child_index)
            : node_id(node_id), child_index(child_index) {
        }

        bool operator==(const NodeInfo& rhs) const;
        bool operator!=(const NodeInfo& rhs) const;

        // The index of a node of which children are being visited.
        NodeId node_id;

        // The index of the next child to visit for this node.
        size_t child_index;
    };

    // Nodes that are being visited.
    std::vector<NodeInfo> stack_;

    // Graph that is being visited.
    const TimelineGraph* graph_;

    // Depth of the current node.
    size_t depth_;

};

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee

#endif    // _TIBEE_ANALYSIS_TIMELINEGRAPH_PREORDERITERATOR_HPP
