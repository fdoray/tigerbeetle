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
#ifndef _TIBEE_ANALYSIS_TIMELINEGRAPH_TIMELINEGRAPH_HPP
#define _TIBEE_ANALYSIS_TIMELINEGRAPH_TIMELINEGRAPH_HPP

#include <memory>
#include <vector>

#include "analysis/timeline_graph/PreorderIterator.hpp"
#include "analysis/timeline_graph/Node.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

class TimelineGraph {
 public:
    TimelineGraph();
    ~TimelineGraph();

    Node& CreateNode();
    
    const Node& GetNode(NodeId node_id) const;
    Node& GetNode(NodeId node_id);

    size_t size() const { return nodes_.size(); }
    bool empty() const { return size() == 0; }

    PreorderIterator preorder_begin() const;
    PreorderIterator preorder_end() const;
    PreorderIterator preorder_from(NodeId node_id) const;

 private:
    typedef std::vector<std::unique_ptr<Node> > NodeVector;

    NodeVector nodes_;
};

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee

#endif    // _TIBEE_ANALYSIS_TIMELINEGRAPH_TIMELINEGRAPH_HPP
