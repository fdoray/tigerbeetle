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
#include "analysis/timeline_graph/PreorderIterator.hpp"

#include "analysis/timeline_graph/TimelineGraph.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

PreorderIterator::PreorderIterator(const TimelineGraph* graph)
    : graph_(graph), depth_(0)
{
}

PreorderIterator::PreorderIterator(const TimelineGraph* graph,
                                   NodeId start_node)
    : graph_(graph), depth_(0)
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
        if (stack_.back().child_index == Node::kChildHorizontal)
            --depth_;
    }

    return *this;
}

bool PreorderIterator::operator==(
    const PreorderIterator& rhs) const
{
    return graph_ == rhs.graph_ && stack_ == rhs.stack_;
}

bool PreorderIterator::operator!=(
    const PreorderIterator& rhs) const
{
    return graph_ != rhs.graph_ || stack_ != rhs.stack_;
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
    return depth_;
}

bool PreorderIterator::MoveToNextChild()
{
    assert(!stack_.empty());

    const Node& current_node = *(*this);

    for (size_t i = static_cast<size_t>(stack_.back().child_index);
             i < Node::kChildCount;
             ++i) {
        NodeId child_id =
                current_node.child(static_cast<Node::ChildIndex>(i));
        ++stack_.back().child_index;
        if (child_id != kInvalidNodeId) {
            stack_.push_back(NodeInfo(child_id, 0));
            if (i == Node::kChildVertical)
                ++depth_;
            return true;
        }
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

}    // namespace timeline_graph
}    // namespace analysis
}    // namespace tibee
