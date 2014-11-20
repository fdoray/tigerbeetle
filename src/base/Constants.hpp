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
#ifndef _TIBEE_BASE_CONSTANTS_HPP
#define _TIBEE_BASE_CONSTANTS_HPP

#include <stdint.h>

namespace tibee
{

// A huge cost.
const uint64_t kHugeCost = -1;

// Services.
extern const char kExecutionBuilderServiceName[];
extern const char kQuarksServiceName[];
extern const char kCurrentStateServiceName[];
extern const char kThreadStatusServiceName[];

// Notifications.
extern const char kExecutionBuilderNotificationPrefix[];
extern const char kExecutionBuilderNotificationName[];

extern const char kCurrentStateNotificationPrefix[];
extern const char kCurrentStateAttributeValueField[];
extern const char kCurrentStateAttributeKeyField[];

extern const char kTraceNotificationPrefix[];
extern const char kTimestampNotificationName[];
extern const char kBeginNotificationName[];
extern const char kEndNotificationName[];

// State system.
extern const char kStateLinux[];
extern const char kStateThreads[];
extern const char kStateCpus[];
extern const char kStateCurCpu[];
extern const char kStateCurThread[];
extern const char kStateResources[];
extern const char kStateIrqs[];
extern const char kStateSoftIrqs[];
extern const char kStateSyscall[];
extern const char kStateStatus[];
extern const char kStatePpid[];
extern const char kStateExecName[];
extern const char kStateIdle[];
extern const char kStateRunUsermode[];
extern const char kStateRunSyscall[];
extern const char kStateIrq[];
extern const char kStateSoftIrq[];
extern const char kStateUnknown[];
extern const char kStateWaitBlocked[];
extern const char kStateInterrupted[];
extern const char kStateWaitForCpu[];
extern const char kStateRaised[];
extern const char kStateSysClone[];

// Graph.
extern const char kStartTime[];
extern const char kEndTime[];
extern const char kDuration[];
extern const char kNodeType[];
extern const char kParentTid[];
extern const char kTid[];
extern const char kArrow[];
extern const char kArrowStart[];
extern const char kStackDepth[];

// Graph: performance counters.
extern const char kInstructions[];
extern const char kCacheReferences[];
extern const char kCacheMisses[];
extern const char kBranchInstructions[];
extern const char kBranches[];
extern const char kBranchMisses[];
extern const char kBranchLoads[];
extern const char kBranchLoadMisses[];
extern const char kPageFault[];
extern const char kFaults[];
extern const char kMajorFaults[];
extern const char kMinorFaults[];

// State history.
extern const char kStateHistoryDirectory[];
extern const char kHistoryTreeFilename[];
extern const char kAttributeTreeFilename[];
extern const char kQuarkDatabaseFilename[];
extern const char kThreadStatusFilename[];

// Invalid.
const uint32_t kInvalidThread = -1;

}

#endif // _TIBEE_BASE_CONSTANTS_HPP
