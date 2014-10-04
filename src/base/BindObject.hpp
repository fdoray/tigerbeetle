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
#ifndef _TIBEE_BASE_BINDOBJECT_HPP
#define _TIBEE_BASE_BINDOBJECT_HPP

#include <functional>

namespace tibee
{
namespace base
{

// Inspired from
// http://stackoverflow.com/questions/14803112/short-way-to-stdbind-member-function-to-object-instance-without-binding-param
template<typename R, typename B, typename... Args>
std::function<R(Args...)> BindObject(R (B::* method)(Args...), B* object) {
    return [=](Args... args){ return (object->*method)(args...); };
}

}
}

#endif // _TIBEE_BASE_BINDOBJECT_HPP
