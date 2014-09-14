/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _TIBEE_TRACE_TRACEUTILS_HPP
#define _TIBEE_TRACE_TRACEUTILS_HPP

#include <cstdint>

#include "trace/BasicTypes.hpp"

namespace tibee
{
namespace trace
{

class TraceUtils
{
public:
    static event_id_t tibeeEventIdFromCtf(std::uint64_t streamId,
                                          std::uint64_t eventId)
    {
        return static_cast<event_id_t>((streamId << 20) | (eventId & 0xfffff));
    }
};

}
}

#endif // _TIBEE_TRACE_TRACEUTILS_HPP
