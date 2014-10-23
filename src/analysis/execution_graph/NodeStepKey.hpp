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
#ifndef _TIBEE_ANALYSIS_EXECUTIONGRAPH_NODESTEPKEY_HPP
#define _TIBEE_ANALYSIS_EXECUTIONGRAPH_NODESTEPKEY_HPP

#include "analysis/execution_graph/Node.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

/**
 * Node step key.
 *
 * A node step is a portion of a node that is between
 * the execution of child nodes.
 *
 * @author Francois Doray
 */
class NodeStepKey
{
public:
    NodeStepKey(NodeId node_id, size_t step_index)
        : node_id_(node_id), step_index_(step_index) {}

    NodeId node_id() const { return node_id_; }
    size_t step_index() const { return step_index_; }

private:
    // Identifier of the node.
    NodeId node_id_;

    // Index of the step. 0 is the first step.
    size_t step_index_;
};

}    // namespace execution_graph
}    // namespace analysis
}    // namespace tibee

#endif    // _TIBEE_ANALYSIS_EXECUTIONGRAPH_NODESTEPKEY_HPP
