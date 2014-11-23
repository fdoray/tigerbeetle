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
#include "critical/CriticalPath.hpp"

namespace tibee
{
namespace critical
{

CriticalPathSegment::CriticalPathSegment()
    : ts(0), tid(-1), type(CriticalEdgeType::kUnknown)
{
}

CriticalPathSegment::CriticalPathSegment(
    timestamp_t ts, uint32_t tid, CriticalEdgeType type)
    : ts(ts), tid(tid), type(type)
{
}

CriticalPath::CriticalPath()
{
}

void CriticalPath::Push(const CriticalPathSegment& segment)
{
    _path.push_back(segment);
}

}
}
