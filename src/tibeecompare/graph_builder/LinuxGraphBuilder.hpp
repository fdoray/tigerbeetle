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
#ifndef _GRAPHBUILDER_LINUXGRAPHBUILDER_HPP
#define _GRAPHBUILDER_LINUXGRAPHBUILDER_HPP

#include <boost/unordered_map.hpp>

#include <common/pipeline/ISink.hpp>
#include <common/state/StateChangeNotification.hpp>
#include <common/traceplayback/AbstractTracePlaybackListener.hpp>
#include <common/traceplayback/EventHandlerSelector.hpp>
#include <tibeecompare/graph_builder/GraphProperties.hpp>
#include <timeline_graph/timeline_graph.h>
#include "GraphBuilder.hpp"

namespace tibee
{

/**
 * Builds a graph from Linux kernel events.
 *
 * @author Francois Doray
 */
class LinuxGraphBuilder
    : public common::AbstractTracePlaybackListener,
      public common::ISink<common::StateChangeNotification>
{
public:
    LinuxGraphBuilder(GraphBuilder* graphBuilder);
    ~LinuxGraphBuilder();

    void SetCurrentState(common::CurrentState* currentState) {
        _currentState = currentState;
    }

private:
    virtual void Receive(const common::StateChangeNotification& notification) override;

    bool onStartImpl(const common::TraceSet* traceSet);
    void onEventImpl(const common::Event& event);
    bool onStopImpl();

    bool onSysExecve(const common::Event& event);
    bool onSchedProcessFork(const common::Event& event);
    bool onSchedProcessExit(const common::Event& event);

    // Forwards events to the appropriate handling method.
    tibee::common::EventHandlerSelector _eventHandlerSelector;

    // Graph builder.
    GraphBuilder* _graphBuilder;

    // Current state.
    common::CurrentState* _currentState;

};

}

#endif // _GRAPHBUILDER_LINUXGRAPHBUILDER_HPP
