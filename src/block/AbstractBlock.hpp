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
#ifndef _TIBEE_BLOCK_ABSTRACTBLOCK_HPP
#define _TIBEE_BLOCK_ABSTRACTBLOCK_HPP

#include "block/BlockInterface.hpp"

namespace tibee
{
namespace block
{

/**
 * Abstract block.
 *
 * @author Francois Doray
 */
class AbstractBlock : public BlockInterface
{
public:
    AbstractBlock();

    virtual void GetNotificationSinks(notification::NotificationCenter* notificationCenter) override;
    virtual void RegisterNotificationObservers(notification::NotificationCenter* notificationCenter) override;
    virtual void RegisterServices(ServiceList* serviceList) override;

    virtual void Start(const value::Value* parameters, const ServiceList& serviceList) override;
    virtual void Execute(const ServiceList& serviceList) override;
    virtual void Stop(const ServiceList& serviceList) override;
};

}
}

#endif // _TIBEE_BLOCK_ABSTRACTBLOCK_HPP
