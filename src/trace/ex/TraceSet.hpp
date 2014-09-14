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
#ifndef _TIBEE_TRACE_EX_TRACESET_HPP
#define _TIBEE_TRACE_EX_TRACESET_HPP

#include <string>
#include <stdexcept>
#include <cstddef>

namespace tibee
{
namespace trace
{
namespace ex
{

class TraceSet :
    public std::runtime_error
{
public:
    TraceSet(const std::string& msg) :
        std::runtime_error {msg}
    {
    }
};

}
}
}

#endif // _TIBEE_TRACE_EX_TRACESET_HPP
