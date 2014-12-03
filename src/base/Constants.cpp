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

const char kExecutionBuilderServiceName[] = "graph-builder";
const char kQuarksServiceName[] = "quarks";
const char kCurrentStateServiceName[] = "current-state";

const char kExecutionBuilderNotificationPrefix[] = "graph-builder";
const char kExecutionBuilderNotificationName[] = "graph";

const char kCurrentStateNotificationPrefix[] = "current-state";
const char kCurrentStateAttributeValueField[] = "key";
const char kCurrentStateAttributeKeyField[] = "value";

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

const char kStateHistoryDirectory[] = "history";
const char kHistoryTreeFilename[] = "-history-tree";
const char kAttributeTreeFilename[] = "-attribute-tree";
const char kQuarkDatabaseFilename[] = "-quarks";
const char kCriticalGraphFilename[] = "-critical-graph";
const char kCriticalPathFilename[] = "-critical-path";

}  // namespace tibee
