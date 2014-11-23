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
#ifndef _TIBEE_CRITICAL_CRITICALTYPES_HPP
#define _TIBEE_CRITICAL_CRITICALTYPES_HPP

#include <stddef.h>

namespace tibee
{
namespace critical
{

typedef size_t CriticalEdgeId;

const CriticalEdgeId kInvalidCriticalEdgeId = -1;

enum CriticalEdgePosition {
    kCriticalEdgeOutVertical = 0,
    kCriticalEdgeInVertical = 1,
    kCriticalEdgeOutHorizontal = 2,
    kCriticalEdgeInHorizontal = 3,
    kCriticalEdgePositionCount = 4,
};

enum class CriticalEdgeType {
    kRun,             // Run in user or system mode.
    kWaitOtherThread, // Waiting for another thread.
    kUnknown,         // Unknown type.
    kVertical,        // Vertical edge.
};

}
}

#endif // _TIBEE_CRITICAL_CRITICALTYPES_HPP
