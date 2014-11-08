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
#include "metric_blocks/AbstractMetricBlock.hpp"

#include "base/Constants.hpp"
#include "block/ServiceList.hpp"

namespace tibee {
namespace metric_blocks {

AbstractMetricBlock::AbstractMetricBlock()
{
}

void AbstractMetricBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kExecutionBuilderServiceName,
                             reinterpret_cast<void**>(&_executionBuilder));

    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));
}

}  // namespace metric_blocks
}  // namespace tibee
