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

#ifndef _TIBEE_COMMON_MAKECALLBACKSINK_HPP
#define _TIBEE_COMMON_MAKECALLBACKSINK_HPP

#include <common/pipeline/CallbackSink.hpp>

namespace tibee
{
namespace common
{

/*
 * Constructs a CallbackSink object.
 *
 * @param object The object on which the callback is invoked.
 * @param method The callback method.
 */
template <typename T, typename O>
inline CallbackSink<T, O> MakeCallbackSink(
    O* object, void (O::*method)(const T&))
{
    return CallbackSink<T, O>(object, method);
}

}  // namespace common
}  // namespace tibee

#endif  // _TIBEE_COMMON_MAKECALLBACKSINK_HPP
