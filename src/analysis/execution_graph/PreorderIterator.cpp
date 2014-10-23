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
#include "analysis/execution_graph/PreorderIterator.hpp"

#include <assert.h>
#include <iostream>

#include "analysis/execution_graph/Graph.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

PreorderIterator::PreorderIterator(const Graph* graph)
    : graph_(graph)
{
}

PreorderIterator::PreorderIterator(const Graph* graph,
                                   NodeId start_node)
    : graph_(graph)
{
    stack_.push(NodeInfo(start_node, 0));
}

PreorderIterator::~PreorderIterator() {
}

PreorderIterator& PreorderIterator::operator++() {
    assert(!stack_.empty());

    while (!stack_.empty()) {
        if (MoveToNextChild())
            break;
        stack_.pop();
    }

    return *this;
}

bool PreorderIterator::operator==(
    const PreorderIterator& rhs) const
{
    return graph_ == rhs.graph_ &&
        stack_.size() == rhs.stack_.size() &&
        (stack_.empty() || stack_.top() == rhs.stack_.top());
}

bool PreorderIterator::operator!=(
    const PreorderIterator& rhs) const
{
    return !(*this == rhs);
}

const Node& PreorderIterator::operator*()
{
    assert(!stack_.empty());
    return graph_->GetNode(stack_.top().node_id);
}

const Node* PreorderIterator::operator->()
{
    assert(!stack_.empty());
    return &graph_->GetNode(stack_.top().node_id);
}

size_t PreorderIterator::Depth() const
{
    return stack_.size() - 1;
}

bool PreorderIterator::MoveToNextChild()
{
    assert(!stack_.empty());

    const Node& current_node = *(*this);
    NodeId child_index = stack_.top().child_index;

    if (child_index < current_node.NumChildren())
    {
        ++stack_.top().child_index;
        stack_.push(NodeInfo(current_node.GetChild(child_index), 0));
        return true;
    }

    return false;
}

bool PreorderIterator::NodeInfo::operator==(const NodeInfo& rhs) const
{
    return node_id == rhs.node_id && child_index == rhs.child_index;
}

bool PreorderIterator::NodeInfo::operator!=(const NodeInfo& rhs) const
{
    return node_id != rhs.node_id || child_index != rhs.child_index;
}

}    // namespace execution_graph
}    // namespace analysis
}    // namespace tibee
