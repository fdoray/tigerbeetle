/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _STATEHISTORYBUILDER_HPP
#define _STATEHISTORYBUILDER_HPP

#include <vector>
#include <memory>
#include <boost/filesystem.hpp>

#include <common/state/CurrentState.hpp>
#include <common/trace/TraceSet.hpp>
#include <common/trace/Event.hpp>
#include "AbstractCacheBuilder.hpp"
#include <common/pipeline/ISink.hpp>
#include <common/state/StateChangeNotification.hpp>
#include <common/stateprov/AbstractStateProvider.hpp>
#include <common/stateprov/StateProviderConfig.hpp>

namespace tibee
{

/**
 * State history builder.
 *
 * An instance of this class is responsible for building the state
 * history on disk during a trace playback.
 *
 * @author Philippe Proulx
 */
class StateHistoryBuilder :
    public AbstractCacheBuilder
{
public:
    /**
     * Builds a state history builder.
     *
     * @param dbDir     Cache directory
     * @param providers List of state providers configurations
     */
    StateHistoryBuilder(const boost::filesystem::path& dbDir,
                        const std::vector<common::StateProviderConfig>& providers);

    ~StateHistoryBuilder();

    /**
     * Returns the number of state changes so far.
     *
     * @returns State changes so far
     */
    std::size_t getStateChanges() const;

    void SetStateChangeSink(common::ISink<common::StateChangeNotification>* sink) {
        _sink = sink;
    }

private:
    bool onStartImpl(const common::TraceSet* traceSet);
    void onEventImpl(const common::Event& event);
    bool onStopImpl();

private:
    common::ISink<common::StateChangeNotification>* _sink;
    std::vector<common::StateProviderConfig> _providersConfigs;
    std::vector<common::AbstractStateProvider::UP> _providers;
    std::unique_ptr<common::CurrentState> _currentState;
};

}

#endif // _STATEHISTORYBUILDER_HPP
