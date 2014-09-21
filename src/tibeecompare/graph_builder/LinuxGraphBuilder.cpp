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

#include <iostream>
#include <stdlib.h>

#include <common/state/CurrentState.hpp>
#include <common/state/StateNode.hpp>
#include <common/utils/print.hpp>
#include <tibeecompare/ex/InvalidTrace.hpp>
#include "LinuxGraphBuilder.hpp"

#define THIS_MODULE "linuxgraphbuilder"

namespace tibee
{

using common::tbmsg;
using common::tbwarn;
using common::tbendl;
using timeline_graph::TimelineGraph;

namespace {

const char kStartTimestampPropertyName[] = "start-ts";
const char kDurationPropertyName[] = "duration";

const char kRunSyscall[] = "syscall";
const char kRunUserMode[] = "usermode";
const char kInterrupted[] = "interrupted";
const char kWaitBlocked[] = "wait-blocked";
const char kWaitForCpu[] = "wait-for-cpu";

}

LinuxGraphBuilder::LinuxGraphBuilder(GraphBuilder* graphBuilder)
    : _graphBuilder(graphBuilder),
      _currentState(nullptr) {
}

LinuxGraphBuilder::~LinuxGraphBuilder() {
}

void LinuxGraphBuilder::Receive(const common::StateChangeNotification& notification) {
    _graphBuilder->SetTimestamp(_currentState->getCurrentTimestamp());

    std::vector<common::Quark> path;
    notification.stateNode.getPath(&path);

    // Thread status change.
    if (path.size() == 4 &&
        path[0] == _currentState->getQuark("linux") &&
        path[1] == _currentState->getQuark("threads") &&
        path[3] == _currentState->getQuark("status")) {
        uint64_t tid = atoll(_currentState->getString(path[2]).c_str());

        // Stop the timer of the previous state.
        if (!notification.stateNode.isNull()) {
            auto previousStateQuark = notification.stateNode.asQuark();
            const std::string& previousState =
                _currentState->getString(previousStateQuark);

            _graphBuilder->StopTimer(tid, previousState);
        }

        // Start the timer of the next state.
        const std::string& nextState =
            _currentState->getString(notification.newValue.asQuark());
        _graphBuilder->StartTimer(tid, nextState);
    }
}

bool LinuxGraphBuilder::onStartImpl(const common::TraceSet* traceSet) {
   using namespace std::placeholders; 

  _eventHandlerSelector.setTraceSet(traceSet);
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sys_execve",
      std::bind(&LinuxGraphBuilder::onSysExecve, this, _1));
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sched_process_fork",
      std::bind(&LinuxGraphBuilder::onSchedProcessFork, this, _1));
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sched_process_exit",
      std::bind(&LinuxGraphBuilder::onSchedProcessExit, this, _1));

  return true;
}

void LinuxGraphBuilder::onEventImpl(const common::Event& event) {
  _graphBuilder->SetTimestamp(event.getTimestamp());
  _eventHandlerSelector.onEvent(event);
}

bool LinuxGraphBuilder::onStopImpl() {
  return true;
}

bool LinuxGraphBuilder::onSysExecve(const common::Event& event) {
    std::string filename = event["filename"].asString();
    uint64_t cpu = event.getStreamPacketContext()["cpu_id"].asUint();

    if (filename == "/usr/local/bin/wk-tasks") {
        auto tid = _currentState->getRoot()
            ["linux"]["cpus"][std::to_string(cpu)]["cur-thread"].asSint32();
        if (!_graphBuilder->AddGraph(tid))
            tbwarn() << "unable to create graph for tid " << tid << tbendl();

        // Start timer for the initial state.
        auto& threadStateNode = _currentState->getRoot()
            ["linux"]["threads"][std::to_string(tid)]["status"];
        if (!threadStateNode.isNull()) {
          auto threadStateQuark = threadStateNode.asQuark();
          const std::string& threadState =
              _currentState->getString(threadStateQuark);
          _graphBuilder->StartTimer(tid, threadState);
        }
    }

    return true;
}

bool LinuxGraphBuilder::onSchedProcessFork(const common::Event& event) {
  uint64_t parent_tid = event["parent_tid"].asUint();
  uint64_t child_tid = event["child_tid"].asUint();

  _graphBuilder->AddTask(parent_tid, child_tid);

  return true;
}

bool LinuxGraphBuilder::onSchedProcessExit(const common::Event& event) {
  uint64_t tid = event["tid"].asUint();
  _graphBuilder->EndTask(tid);

  return true;
}

}
