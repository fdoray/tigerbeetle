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
#ifndef _TIBEE_STATEBLOCKS_CURRENTSTATEBLOCK_HPP
#define _TIBEE_STATEBLOCKS_CURRENTSTATEBLOCK_HPP

#include <vector>

#include "block/AbstractBlock.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"
#include "quark/StringQuarkDatabase.hpp"
#include "state/CurrentState.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace state_blocks
{

/**
 * A block that keeps track of the current state.
 *
 * @author Francois Doray
 */
class CurrentStateBlock : public block::AbstractBlock
{
public:
    static const char* kCurrentStateServiceName;
    static const char* kQuarksServiceName;
    static const char* kAttributeKeyField;
    static const char* kAttributeValueField;
    static const char* kNotificationPrefix;

    CurrentStateBlock();

    virtual void RegisterServices(block::ServiceList* serviceList) override;
    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onTimestamp(const notification::Path& path, const value::Value* value);
    void onStateChange(state::AttributeKey attribute, const value::Value* value);

    quark::StringQuarkDatabase::UP _quarks;
    state::CurrentState::UP _currentState;

    typedef std::vector<const notification::NotificationSink*> Sinks;
    Sinks _sinks;

    notification::NotificationCenter* _notificationCenter;
};

}
}

#endif // _TIBEE_TRACEBLOCKS_TRACEBLOCK_HPP
