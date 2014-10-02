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
#include "notification/Path.hpp"
#include "notification/Token.hpp"
#include "state/AttributeTree.hpp"
#include "value/Utils.hpp"

namespace tibee
{
namespace state_blocks
{

namespace pl = std::placeholders;

const char* CurrentStateBlock::kCurrentStateServiceName = "currentState";
const char* CurrentStateBlock::kAttributeKeyField = "key";
const char* CurrentStateBlock::kAttributeValueField = "value";
const char* CurrentStateBlock::kStateNotificationPrefix = "state";

CurrentStateBlock::CurrentStateBlock()
    : _currentState(std::bind(&CurrentStateBlock::onStateChange,
                              this,
                              pl::_1,
                              pl::_2)),
      _notificationCenter(nullptr)
{
}

void CurrentStateBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kCurrentStateServiceName, &_currentState);
}

void CurrentStateBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(
        notification::NotificationCenter::kNotificationCenterServiceName,
        reinterpret_cast<void**>(&_notificationCenter));
}

void CurrentStateBlock::onStateChange(state::AttributeKey attribute, const value::Value* value)
{
    assert(_notificationCenter != nullptr);

    if (attribute.get() >= _sinks.size())
        _sinks.resize(attribute.get() + 1);

    if (_sinks[attribute.get()] == nullptr)
    {
        state::AttributeTree::Path path;
        _currentState.GetAttributePath(attribute, &path);
        notification::Path notificationPath {notification::Token { kStateNotificationPrefix } };
        for (const auto& quark : path)
            notificationPath.push_back(notification::Token { _currentState.String(quark) });

        _sinks[attribute.get()] = _notificationCenter->GetSink(notificationPath);
    }

    value::StructValue::UP notification {new value::StructValue};
    notification->AddField<value::UIntValue>(kAttributeKeyField, attribute.get());

    if (value != nullptr)
        notification->AddField(kAttributeValueField, value->Copy());
    else
        notification->AddField(kAttributeValueField, nullptr);

    _sinks[attribute.get()]->PostNotification(notification.get());
}

}
}
