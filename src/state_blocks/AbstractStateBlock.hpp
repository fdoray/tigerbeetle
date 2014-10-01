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
#ifndef _TIBEE_STATEBLOCKS_ABSTRACTSTATEBLOCK_HPP
#define _TIBEE_STATEBLOCKS_ABSTRACTSTATEBLOCK_HPP

#include <functional>
#include <string>
#include <vector>

#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
#include "state/CurrentState.hpp"
#include "state_blocks/MakeNotification.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee
{
namespace state_blocks
{

/**
 * An abstract state block.
 *
 * A state block receives events and produces state changes.
 *
 * @author Francois Doray
 */
class AbstractStateBlock : public block::AbstractBlock
{
public:
    AbstractStateBlock();

    virtual void GetNotificationSinks(notification::NotificationCenter* notificationCenter) = 0;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) = 0;

    virtual void LoadServices(const block::ServiceList& serviceList) override;


protected:
    typedef std::function<void (const trace::EventValue&)> EventHandler;
    void AddKernelObserver(notification::NotificationCenter* notificationCenter,
                           notification::Token token,
                           EventHandler eventHandler);
    void AddUstObserver(notification::NotificationCenter* notificationCenter,
                        notification::Token token,
                        EventHandler eventHandler);

    state::CurrentState* CurrentState() { return _currentState; }

private:
    void onEvent(const value::Value* event, EventHandler handler);

    state::CurrentState* _currentState;

    typedef std::vector<notification::NotificationSink*> Sinks;
    Sinks _sinks;

};

}
}

#endif // _TIBEE_STATEBLOCKS_ABSTRACTSTATEBLOCK_HPP
