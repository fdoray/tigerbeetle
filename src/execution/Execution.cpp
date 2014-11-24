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
#include "execution/Execution.hpp"

#include "base/BasicTypes.hpp"
#include "base/print.hpp"

namespace tibee {
namespace execution {

using base::tbendl;
using base::tberror;

Execution::Execution(const std::string& description)
        : _description(description) {}

const value::Value* Execution::GetMetric(quark::Quark name) const
{
    auto look = _metrics.find(name);
    if (look == _metrics.end())
        return nullptr;
    return look->second.get();
}

value::Value* Execution::GetMetric(quark::Quark name)
{
    auto look = _metrics.find(name);
    if (look == _metrics.end())
        return nullptr;
    return look->second.get();
}

void Execution::SetMetric(quark::Quark name, value::Value::UP value)
{
    _metrics[name] = std::move(value);
}

NodeId Execution::GetLastNode(quark::Quark qEndTime) const
{
    timestamp_t last_ts = 0;
    NodeId last_node_id = kInvalidNodeId;

    for (NodeId nodeId = 0; nodeId < graph().size(); ++nodeId)
    {
        // Get end time for this node.
        auto end_time_value = node_properties().GetProperty(NodeStepKey(nodeId, 0), qEndTime);
        if (end_time_value == nullptr)
        {
            tberror() << "Expected an end time for node " << nodeId << "." << tbendl();
            continue;
        }

        timestamp_t end_time = end_time_value->AsULong();

        if (end_time >= last_ts)
        {
            last_node_id = nodeId;
            last_ts = end_time;
        }
    }

    return last_node_id;
}

uint32_t Execution::TidForNode(quark::Quark qTid, NodeId nodeId) const
{
    std::vector<int32_t> tidForDepth;

    auto node_it = graph().preorder_begin();
    auto node_it_end = graph().preorder_end();

    for (; node_it != node_it_end; ++node_it)
    {
        auto curNodeId = node_it->id();
        size_t depth = node_it.Depth();

        // Tid.
        int32_t tid = 0;
        auto tidValue = node_properties().GetProperty(NodeStepKey(curNodeId, 0), qTid);
        if (tidValue != nullptr)
        {
            tid = tidValue->AsLong();
            tidForDepth.resize(depth + 1);
            tidForDepth.back() = tid;
        }
        else
        {
            if (tidForDepth.size() < depth)
            {
                tberror() << "No tid in stack..." << tbendl();
            }
            else
            {
                tid = tidForDepth[depth - 1];
                tidForDepth.resize(depth + 1);
                tidForDepth.back() = tid;
            }
        }

        if (curNodeId == nodeId)
        {
            return tid;
        }
    }

    return -1;
}

void Execution::ExtractTids(quark::Quark qTid, std::unordered_set<uint32_t>* tids) const
{
    assert(tids);
    assert(tids->empty());

    for (size_t i = 0; i < graph().size(); ++i)
    {
        auto tidValue = node_properties().GetProperty(NodeStepKey(i, 0), qTid);
        if (tidValue != nullptr)
        {
            uint32_t tid = tidValue->AsUInteger();
            tids->insert(tid);
        }
    }
}


}  // namespace execution
}  // namespace tibee
