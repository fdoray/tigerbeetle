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
#include "state_blocks/CurrentStateBlock.hpp"

#include <iostream>

#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "value/Utils.hpp"

namespace tibee
{
namespace state_blocks
{

const char* CurrentStateBlock::kCurrentStateServiceName = "currentState";
const char* CurrentStateBlock::kAttributeKeyField = "key";
const char* CurrentStateBlock::kAttributeValueField = "value";

CurrentStateBlock::CurrentStateBlock()
{
}

void CurrentStateBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kCurrentStateServiceName, &_currentState);
}

void CurrentStateBlock::RegisterNotificationObservers(notification::NotificationCenter* notificationCenter)
{
    namespace pl = std::placeholders;

    notificationCenter->RegisterNotificationObserver(
        notification::KeyPath {notification::Token("state")},
        std::bind(&CurrentStateBlock::onStateChange, this, pl::_1, pl::_2)
    );
}

void CurrentStateBlock::onStateChange(const notification::KeyPath& path, const value::Value* value)
{
    state::AttributeKey key {value->GetField(kAttributeKeyField)->AsUInteger()};
    _currentState.SetAttribute(key, value->GetField(kAttributeValueField)->Copy());
}

}
}
