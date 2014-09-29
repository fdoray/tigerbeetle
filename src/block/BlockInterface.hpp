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
#ifndef _TIBEE_BLOCK_BLOCKINTERFACE_HPP
#define _TIBEE_BLOCK_BLOCKINTERFACE_HPP

#include <memory>

namespace tibee
{

namespace notification
{
// Forward declaration.
class NotificationCenter;
}

namespace value
{
// Forward declaration.
class Value;
}

namespace block
{

// Forward declaration.
class ServiceList;

/**
 * Block interface.
 *
 * @author Francois Doray
 */
class BlockInterface
{
public:
    typedef std::unique_ptr<BlockInterface> UP;

    virtual ~BlockInterface();

    virtual void Start(const value::Value* parameters) = 0;

    virtual void GetNotificationSinks(notification::NotificationCenter* notificationCenter) = 0;
    virtual void RegisterNotificationObservers(notification::NotificationCenter* notificationCenter) = 0;
    virtual void RegisterServices(ServiceList* serviceList) = 0;
    virtual void LoadServices(const ServiceList& serviceList) = 0;

    virtual void Execute() = 0;
    virtual void Stop() = 0;
};

}
}

#endif // _TIBEE_BLOCK_BLOCKINTERFACE_HPP
