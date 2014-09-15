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

#ifndef _TIBEE_COMMON_STATECHANGENOTIFICATION_HPP
#define _TIBEE_COMMON_STATECHANGENOTIFICATION_HPP

namespace tibee
{
namespace common
{

class CurrentState;
class StateNode;
class AbstractStateValue;

struct StateChangeNotification {
    StateChangeNotification(
        const CurrentState& currentState,
        const StateNode& stateNode,
        const AbstractStateValue& newValue) :
        currentState(currentState),
        stateNode(stateNode),
        newValue(newValue)
    {
    }

    const CurrentState& currentState;
    const StateNode& stateNode;
    const AbstractStateValue& newValue;
};

}  // namespace common
}  // namespace tibee

#endif  // _TIBEE_COMMON_STATECHANGENOTIFICATION_HPP
