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
#ifndef _TIBEE_BLOCK_SERVICELIST_HPP
#define _TIBEE_BLOCK_SERVICELIST_HPP

#include <boost/utility.hpp>
#include <unordered_map>

namespace tibee
{
namespace block
{

/**
 * Service list.
 *
 * @author Francois Doray
 */
class ServiceList :
    boost::noncopyable
{
public:
    ServiceList();
    ~ServiceList();

    void AddService(const std::string& serviceName,
                    void* serviceObject);

    bool QueryService(const std::string& serviceName,
                      void** serviceObject) const;

private:
    typedef std::unordered_map<std::string, void*> ServiceMap;
    ServiceMap _services;
};

}
}

#endif // _TIBEE_BLOCK_SERVICELIST_HPP
