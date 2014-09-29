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

#include "block/ServiceList.hpp"

#include <assert.h>

namespace tibee
{
namespace block
{

ServiceList::ServiceList()
{
}

ServiceList::~ServiceList()
{
}

void ServiceList::AddService(const std::string& serviceName,
                             void* serviceObject)
{
    assert(_services.find(serviceName) == _services.end());
    _services[serviceName] = serviceObject;
}

bool ServiceList::QueryService(const std::string& serviceName,
                               void** serviceObject) const
{
    auto look = _services.find(serviceName);
    if (look == _services.end())
        return false;
    *serviceObject = look->second;
    return true;
}

}
}
