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

#ifndef _TIBEE_COMMON_MAKEMULTISINK_HPP
#define _TIBEE_COMMON_MAKEMULTISINK_HPP

#include <type_traits>

#include <common/pipeline/MultiSink.hpp>

namespace tibee
{
namespace common
{

/*
 * Constructs a MultiSink object from a container of sinks.
 *
 * @param sinks A container of pointers to sinks to include in the MultiSink.
 * @returns The constructed MultiSink.
 */
template <typename T, typename C>
MultiSink<T> MakeMultiSink(C sinks)
{
    MultiSink<T> multi_sink;
    for (auto sink : sinks) {
        multi_sink.AddSink(sink);
    }
    return multi_sink;
}

}  // namespace common
}  // namespace tibee

#endif  // _TIBEE_COMMON_MAKEMULTISINK_HPP
