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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_CRITICAL_CRITICALNODE_HPP
#define _TIBEE_CRITICAL_CRITICALNODE_HPP

#include <array>
#include <boost/noncopyable.hpp>
#include <memory>

#include "base/BasicTypes.hpp"
#include "critical/CriticalTypes.hpp"

namespace tibee
{
namespace critical
{

/**
 * Critical node.
 *
 * @author Francois Doray
 */
class CriticalNode :
    boost::noncopyable
{
public:
    typedef std::unique_ptr<CriticalNode> UP;

    CriticalNode();
    CriticalNode(timestamp_t ts, uint32_t tid);

    CriticalEdgeId edge(CriticalEdgePosition position) const
    {
        return _edges[static_cast<size_t>(position)];
    }

    void set_edge(CriticalEdgePosition position, CriticalEdgeId edge)
    {
        _edges[static_cast<size_t>(position)] = edge;
    }

    timestamp_t ts() const { return _ts; }
    uint32_t tid() const { return _tid; }

private:
    // Edges.
    std::array<CriticalEdgeId, kCriticalEdgePositionCount> _edges;

    // Timestamp.
    timestamp_t _ts;

    // Thread id.
    uint32_t _tid;
};

}
}

#endif // _TIBEE_CRITICAL_CRITICALNODE_HPP
