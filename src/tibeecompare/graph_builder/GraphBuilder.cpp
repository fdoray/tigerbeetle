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
#include "GraphBuilder.hpp"

#define THIS_MODULE "graphbuilder"

namespace tibee
{

using common::tbmsg;
using common::tbendl;
using timeline_graph::TimelineGraph;

namespace {
const char kStartTimestampPropertyName[] = "start-ts";
const char kLastStateChangeTimestamp[] = "last-state-change-ts";
const char kDurationPropertyName[] = "duration";

const char kRunSyscall[] = "syscall";
const char kRunUserMode[] = "usermode";
const char kInterrupted[] = "interrupted";
const char kWaitBlocked[] = "wait-blocked";
const char kWaitForCpu[] = "wait-for-cpu";

}

GraphBuilder::GraphBuilder() : _currentState(nullptr) {
}

GraphBuilder::~GraphBuilder() {
}

std::unique_ptr<TimelineGraph> GraphBuilder::TakeGraph() {
  return std::move(_graph);
}

std::unique_ptr<NodeProperties> GraphBuilder::TakeProperties() {
  return std::move(_properties);
}

void GraphBuilder::Receive(const common::StateChangeNotification& notification) {
  // Keep a pointer to the current state.
  _currentState = &notification.currentState;

  std::vector<common::Quark> path;
  notification.stateNode.getPath(&path);

  if (path.size() == 4 &&
      path[0] == notification.currentState.getQuark("linux") &&
      path[1] == notification.currentState.getQuark("threads") &&
      path[3] == notification.currentState.getQuark("status")) {
    // Thread status change.
    uint64_t tid = atoll(notification.currentState.getString(path[2]).c_str());
    UpdateTimeThread(tid);
  }
}

void GraphBuilder::UpdateTimeThread(uint64_t tid) {
  if (_last_node_for_tid.find(tid) == _last_node_for_tid.end())
    return;

  timeline_graph::TimelineNodeId nodeid = _last_node_for_tid[tid];

  auto& stateNode = _currentState->getRoot()["linux"]["threads"][tid]["status"];
  auto startTs = _properties->GetProperty(nodeid, kLastStateChangeTimestamp).asUint64();
  auto currentTs = _currentState->getCurrentTimestamp();
  auto duration = currentTs - startTs;

  std::string state = _currentState->getString(stateNode.getValue().asQuark());

  auto& stateTimeValue = _properties->GetProperty(nodeid, state);
  uint64_t stateTime = 0;
  if (stateTimeValue) {
    stateTime = stateTimeValue.asUint64();
  }
  stateTime += duration;

  _properties->SetProperty(nodeid, state, stateTime);
  _properties->SetProperty(nodeid, kLastStateChangeTimestamp, currentTs);
}

bool GraphBuilder::onStartImpl(const common::TraceSet* traceSet) {
   using namespace std::placeholders; 

   _graph = std::unique_ptr<TimelineGraph>(new TimelineGraph());
   _properties = std::unique_ptr<NodeProperties>(new NodeProperties());
   _last_node_for_tid.clear();

  _eventHandlerSelector.setTraceSet(traceSet);
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sys_execve",
      std::bind(&GraphBuilder::onSysExecve, this, _1));
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sched_process_fork",
      std::bind(&GraphBuilder::onSchedProcessFork, this, _1));
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sched_process_exit",
      std::bind(&GraphBuilder::onSchedProcessExit, this, _1));

  return true;
}

void GraphBuilder::onEventImpl(const common::Event& event) {
  _eventHandlerSelector.onEvent(event);
}

bool GraphBuilder::onStopImpl() {
  return true;
}

bool GraphBuilder::onSysExecve(const common::Event& event) {
  std::string filename = event["filename"].asString();
  uint64_t cpu = event.getStreamPacketContext()["cpu_id"].asUint();

  if (filename == "/usr/local/bin/wk-tasks") {
    auto& node = _graph->CreateNode();
    _properties->SetProperty(node.id(),
                             kStartTimestampPropertyName,
                             event.getTimestamp());

    auto tid = _currentState->getRoot()["linux"]["cpus"][std::to_string(cpu)]["cur-thread"].asSint32();
    _last_node_for_tid[tid] = node.id();
    
    _properties->SetProperty(node.id(), kLastStateChangeTimestamp, event.getTimestamp());
  }

  return true;
}

bool GraphBuilder::onSchedProcessFork(const common::Event& event) {
  uint64_t parent_tid = event["parent_tid"].asUint();
  uint64_t child_tid = event["child_tid"].asUint();

  auto parent_last_node_it = _last_node_for_tid.find(parent_tid);
  if (parent_last_node_it == _last_node_for_tid.end())
    return true;

  // Update duration of the last node.
  uint64_t last_node_ts = _properties->GetProperty(
      parent_last_node_it->second,
      kStartTimestampPropertyName).asUint64();
  uint64_t last_node_duration = event.getTimestamp() - last_node_ts;
  _properties->SetProperty(
      parent_last_node_it->second,
      kDurationPropertyName,
      last_node_duration);

  // Create the branch node of the parent thread.
  UpdateTimeThread(parent_tid);
  auto& branch_node = _graph->CreateNode();
  _properties->SetProperty(branch_node.id(),
                           kStartTimestampPropertyName,
                           event.getTimestamp());
  _properties->SetProperty(branch_node.id(),
                           kLastStateChangeTimestamp,
                           event.getTimestamp());
  _graph->GetNode(parent_last_node_it->second).set_horizontal_child(
      branch_node.id());

  // Create the first node of the child thread.
  auto& child_node = _graph->CreateNode();
  _properties->SetProperty(child_node.id(),
                           kStartTimestampPropertyName,
                           event.getTimestamp());
  _properties->SetProperty(child_node.id(),
                           kLastStateChangeTimestamp,
                           event.getTimestamp());
  branch_node.set_vertical_child(child_node.id());

  // Keep track of the last node for each thread.
  _last_node_for_tid[parent_tid] = branch_node.id();
  _last_node_for_tid[child_tid] = child_node.id();

  return true;
}

bool GraphBuilder::onSchedProcessExit(const common::Event& event) {
  uint64_t tid = event["tid"].asUint();

  auto last_node_it = _last_node_for_tid.find(tid);
  if (last_node_it == _last_node_for_tid.end())
    return true;
  auto last_node_id = last_node_it->second;

  // Update duration of the last node.
  uint64_t last_node_ts = _properties->GetProperty(
      last_node_it->second,
      kStartTimestampPropertyName).asUint64();
  uint64_t last_node_duration = event.getTimestamp() - last_node_ts;
  _properties->SetProperty(
      last_node_it->second,
      kDurationPropertyName,
      last_node_duration);

  // Update time.
  UpdateTimeThread(tid);

  // Create the exit node.
  auto& exit_node = _graph->CreateNode();
  _graph->GetNode(last_node_id).set_horizontal_child(exit_node.id());

  // No more data will be logged for this thread.
  _last_node_for_tid.erase(tid);

  return true;
}

}
