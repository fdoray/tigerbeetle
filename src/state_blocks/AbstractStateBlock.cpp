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
#include "state_blocks/AbstractStateBlock.hpp"

#include "base/Constants.hpp"
#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "trace_blocks/TraceBlock.hpp"

namespace tibee
{
namespace state_blocks
{

using notification::Token;

AbstractStateBlock::AbstractStateBlock() :
    _currentState {nullptr}
{
}

void AbstractStateBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));    
}

}
}
