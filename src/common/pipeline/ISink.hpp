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

#ifndef _TIBEE_COMMON_ISINK_HPP
#define _TIBEE_COMMON_ISINK_HPP

namespace tibee
{
namespace common
{

/**
 * Interface for a sink that receives elements of a parameterized type.
 *
 * @author Francois Doray
 */
template <typename T>
class ISink
{
public:
    virtual ~ISink() {}

    /*
     * Receives an element.
     *
     * @param element The received element.
     */
    virtual void Receive(const T& element) = 0;
};

}  // namespace common
}  // namespace tibee

#endif  // _TIBEE_COMMON_ISINK_HPP
