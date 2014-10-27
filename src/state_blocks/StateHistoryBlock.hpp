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
#ifndef _TIBEE_STATEBLOCKS_STATEHISTORYBLOCK_HPP
#define _TIBEE_STATEBLOCKS_STATEHISTORYBLOCK_HPP

#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/HistoryFileSink.hpp>
#include <memory>
#include <vector>

#include "block/AbstractBlock.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"
#include "state/CurrentState.hpp"

namespace tibee
{
namespace state_blocks
{

/**
 * A block that writes a state history.
 *
 * @author Francois Doray
 */
class StateHistoryBlock : public block::AbstractBlock
{
public:
    StateHistoryBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void InitTranslators();
    void onStateChange(const notification::Path& path, const value::Value* value);
    void onEnd(const notification::Path& path, const value::Value* value);

    // Prefix of the name of generated files.
    std::string _filenamePrefix;

    // Current state.
    state::CurrentState* _currentState;

    // Interval history sink.
    std::unique_ptr<delo::HistoryFileSink> _intervalFileSink;

    // (value + ts -> delorean interval) translators
    typedef std::function<delo::AbstractInterval* (uint32_t, const value::Value*, timestamp_t, timestamp_t)> Translator;
    std::array<Translator, 16> _translators;
    Translator _nullTranslator;
};

}
}

#endif // _TIBEE_STATEBLOCKS_STATEHISTORYBLOCK_HPP
