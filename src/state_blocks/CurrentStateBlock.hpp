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

#include "block/AbstractBlock.hpp"
#include "notification/NotificationCenter.hpp"
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
    static const char* kAttributeKeyField;
    static const char* kAttributeValueField;

    CurrentStateBlock();

    virtual void RegisterServices(block::ServiceList* serviceList) override;

private:
    void onStateChange(state::AttributeKey attribute, const value::Value* value);

    state::CurrentState _currentState;
};

}
}

#endif // _TIBEE_TRACEBLOCKS_TRACEBLOCK_HPP
