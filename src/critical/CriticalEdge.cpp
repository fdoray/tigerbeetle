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
#include "critical/CriticalEdge.hpp"

#include <assert.h>

namespace tibee
{
namespace critical
{

CriticalEdge::CriticalEdge()
    : _type(CriticalEdgeType::kUnknown), _from(nullptr), _to(nullptr)
{
}

CriticalEdge::CriticalEdge(CriticalEdgeType type, CriticalNode* from, CriticalNode* to)
    : _type(type), _from(from), _to(to)
{
}

timestamp_t CriticalEdge::Cost() const
{
    assert(from() != nullptr);
    assert(to() != nullptr);

    if (type() == CriticalEdgeType::kBlock)
    {
        return 0;
    }
    assert(to()->ts() >= from()->ts());
    return to()->ts() - from()->ts(); 
}

}
}
