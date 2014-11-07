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
#include "execution/PreorderIterator.hpp"

#include <assert.h>

#include "execution/Graph.hpp"

namespace tibee {
namespace execution {

PreorderIterator::PreorderIterator(const Graph* graph)
    : graph_(graph)
{
}

PreorderIterator::PreorderIterator(const Graph* graph,
                                   NodeId start_node)
    : graph_(graph)
{
    stack_.push_back(NodeInfo(start_node, 0));
}

PreorderIterator::~PreorderIterator() {
}

PreorderIterator& PreorderIterator::operator++() {
    assert(!stack_.empty());

    while (!stack_.empty()) {
        if (MoveToNextChild())
            break;
        stack_.pop_back();
    }

    return *this;
}

bool PreorderIterator::operator==(
    const PreorderIterator& rhs) const
{
    return graph_ == rhs.graph_ &&
        stack_.size() == rhs.stack_.size() &&
        (stack_.empty() || stack_.back() == rhs.stack_.back());
}

bool PreorderIterator::operator!=(
    const PreorderIterator& rhs) const
{
    return !(*this == rhs);
}

const Node& PreorderIterator::operator*()
{
    assert(!stack_.empty());
    return graph_->GetNode(stack_.back().node_id);
}

const Node* PreorderIterator::operator->()
{
    assert(!stack_.empty());
    return &graph_->GetNode(stack_.back().node_id);
}

size_t PreorderIterator::Depth() const
{
    return stack_.size() - 1;
}

NodeId PreorderIterator::ParentNodeId() const
{
    if (Depth() == 0)
        return -1;
    return stack_[stack_.size() - 2].node_id;
}

size_t PreorderIterator::ChildIndex() const
{
    if (Depth() == 0)
        return 0;
    return stack_[stack_.size() - 2].child_index - 1;
}

bool PreorderIterator::MoveToNextChild()
{
    assert(!stack_.empty());

    const Node& current_node = *(*this);
    NodeId child_index = stack_.back().child_index;

    if (child_index < current_node.NumChildren())
    {
        ++stack_.back().child_index;
        stack_.push_back(NodeInfo(current_node.GetChild(child_index), 0));
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

}    // namespace execution
}    // namespace tibee
