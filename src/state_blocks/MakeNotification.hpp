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
#ifndef _TIBEE_STATEBLOCKS_MAKENOTIFICATION_HPP
#define _TIBEE_STATEBLOCKS_MAKENOTIFICATION_HPP

#include "state/AttributeKey.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "value/MakeValue.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace state_blocks
{

template <typename T>
inline value::Value::UP MakeNotification(
    state::AttributeKey key,
    const typename T::ScalarType& value)
{
    value::StructValue::UP notification {new value::StructValue};

    notification->AddField(CurrentStateBlock::kAttributeKeyField,
                           value::MakeValue<value::UIntValue>(key.get()));
    notification->AddField(CurrentStateBlock::kAttributeValueField,
                           value::MakeValue<T>(value));

    return std::move(notification);
}

}
}

#endif // _TIBEE_TRACEBLOCKS_MAKENOTIFICATION_HPP
