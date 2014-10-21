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
#include "base/Constants.hpp"

namespace tibee
{

const char kGraphBuilderServiceName[] = "graph-builder";
const char kQuarksServiceName[] = "quarks";

const char kGraphBuilderNotificationPrefix[] = "graph-builder";
const char kGraphBuilderNotificationName[] = "graph";

const char kTraceNotificationPrefix[] = "event";
const char kTimestampNotificationName[] = "ts";
const char kBeginNotificationName[] = "begin";
const char kEndNotificationName[] = "end";

const char kStateLinux[] = "linux";
const char kStateThreads[] = "threads";
const char kStateCpus[] = "cpus";
const char kStateCurCpu[] = "cur-cpu";
const char kStateCurThread[] = "cur-thread";
const char kStateResources[] = "resources";
const char kStateIrqs[] = "irqs";
const char kStateSoftIrqs[] = "soft-irqs";
const char kStateSyscall[] = "syscall";
const char kStateStatus[] = "status";
const char kStatePpid[] = "ppid";
const char kStateExecName[] = "exec-name";
const char kStateIdle[] = "idle";
const char kStateRunUsermode[] = "usermode";
const char kStateRunSyscall[] = "syscall";
const char kStateIrq[] = "irq";
const char kStateSoftIrq[] = "soft-irq";
const char kStateUnknown[] = "unknown";
const char kStateWaitBlocked[] = "wait-blocked";
const char kStateInterrupted[] = "interrupted";
const char kStateWaitForCpu[] = "wait-for-cpu";
const char kStateRaised[] = "raised";
const char kStateSysClone[] = "sys_clone";

const char kDuration[] = "duration";
const char kNodeType[] = "node-type";
const char kTid[] = "tid";
const char kStartTime[] = "start-time";
const char kArrow[] = "arrow";

const char kInstructions[] = "instructions";
const char kCacheReferences[] = "cache-references";
const char kCacheMisses[] = "cache-misses";
const char kBranchInstructions[] = "branch-instructions";
const char kBranches[] = "branches";
const char kBranchMisses[] = "branch-misses";
const char kBranchLoads[] = "branch-loads";
const char kBranchLoadMisses[] = "branch-load-misses";
const char kPageFault[] = "page-fault";
const char kFaults[] = "faults";
const char kMajorFaults[] = "major-faults";
const char kMinorFaults[] = "minor-faults";

}  // namespace tibee
