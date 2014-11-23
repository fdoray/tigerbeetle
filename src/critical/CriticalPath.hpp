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
#ifndef _TIBEE_CRITICAL_CRITICALPATH_HPP
#define _TIBEE_CRITICAL_CRITICALPATH_HPP

#include <boost/noncopyable.hpp>
#include <vector>

#include "base/BasicTypes.hpp"
#include "critical/CriticalTypes.hpp"

namespace tibee
{
namespace critical
{

/**
 * Critical path segment.
 *
 * @author Francois Doray
 */
struct CriticalPathSegment
{
    CriticalPathSegment();
    CriticalPathSegment(timestamp_t ts, uint32_t tid, CriticalEdgeType type);

    timestamp_t ts;
    uint32_t tid;
    CriticalEdgeType type;
};

/**
 * Critical path.
 *
 * @author Francois Doray
 */
class CriticalPath :
    boost::noncopyable
{
public:
    typedef std::vector<CriticalPathSegment> CriticalPathSegmentVector;

    CriticalPath();

    void Push(const CriticalPathSegment& segment);

    size_t size() const {
        return _path.size();
    }

    CriticalPathSegmentVector::const_iterator begin() const {
        return _path.begin();
    }

    CriticalPathSegmentVector::const_iterator end() const {
        return _path.end();
    }

private:
    // Critical path.
    CriticalPathSegmentVector _path;
};

}
}

#endif // _TIBEE_CRITICAL_CRITICALPATH_HPP
