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
#include "metric_blocks/LinuxPerfThreadMetricBlock.hpp"

#include <stdlib.h>

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "trace/value/EventValue.hpp"
#include "value/MakeValue.hpp"
#include "value/ReadValue.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace metric_blocks {

namespace
{
using notification::Token;

const char kInstructionsField[] = "perf_thread_instructions";
const char kCacheReferencesField[] = "perf_thread_cache_references";
const char kCacheMissesField[] = "perf_thread_cache_misses";
const char kBranchInstructionsField[] = "perf_thread_branch_instructions";
const char kBranchesField[] = "perf_thread_branches";
const char kBranchMissesField[] = "perf_thread_branch_misses";
const char kBranchLoadsField[] = "perf_thread_branch_loads";
const char kBranchLoadMissesField[] = "perf_thread_branch_load_misses";
const char kPageFaultField[] = "perf_thread_page_fault";
const char kFaultsField[] = "perf_thread_faults";
const char kMajorFaultsField[] = "perf_thread_major_faults";
const char kMinorFaultsField[] = "perf_thread_minor_faults";

}  // namespace

LinuxPerfThreadMetricBlock::LinuxPerfThreadMetricBlock()
{
}

void LinuxPerfThreadMetricBlock::LoadServices(const block::ServiceList& serviceList)
{
    AbstractMetricBlock::LoadServices(serviceList);

    // Get constant quarks.
    Q_INSTRUCTIONS = State()->Quark(kInstructions);
    Q_CACHE_REFERENCES = State()->Quark(kCacheReferences);
    Q_CACHE_MISSES = State()->Quark(kCacheMisses);
    Q_BRANCH_INSTRUCTIONS = State()->Quark(kBranchInstructions);
    Q_BRANCHES = State()->Quark(kBranches);
    Q_BRANCH_MISSES = State()->Quark(kBranchMisses);
    Q_BRANCH_LOADS = State()->Quark(kBranchLoads);
    Q_BRANCH_LOAD_MISSES = State()->Quark(kBranchLoadMisses);
    Q_PAGE_FAULT = State()->Quark(kPageFault);
    Q_FAULTS = State()->Quark(kFaults);
    Q_MAJOR_FAULTS = State()->Quark(kMajorFaults);
    Q_MINOR_FAULTS = State()->Quark(kMinorFaults);

    Q_CUR_THREAD = State()->Quark(kStateCurThread);
    Q_STATUS = State()->Quark(kStateStatus);
    Q_INTERRUPTED = State()->Quark(kStateInterrupted);

    // CPUs attribute.
    _cpusAttribute = State()->GetAttributeKey({
        State()->Quark(kStateLinux),
        State()->Quark(kStateCpus)
    });

    // Threads attribute.
    _threadsAttribute = State()->GetAttributeKey({
        State()->Quark(kStateLinux),
        State()->Quark(kStateThreads)
    });
}

void LinuxPerfThreadMetricBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token("lttng-ust")},
        base::BindObject(&LinuxPerfThreadMetricBlock::onEvent, this));
}

void LinuxPerfThreadMetricBlock::onEvent(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);
    auto context = value::StructValueBase::Cast(event->getStreamEventContext());

    auto thread = context->GetField("vtid")->AsUInteger();

    IncrementPerfCounter(thread, Q_INSTRUCTIONS, context->GetField(kInstructionsField));
    IncrementPerfCounter(thread, Q_CACHE_REFERENCES, context->GetField(kCacheReferencesField));
    IncrementPerfCounter(thread, Q_CACHE_MISSES, context->GetField(kCacheMissesField));
    IncrementPerfCounter(thread, Q_BRANCH_INSTRUCTIONS, context->GetField(kBranchInstructionsField));
    IncrementPerfCounter(thread, Q_BRANCHES, context->GetField(kBranchesField));
    IncrementPerfCounter(thread, Q_BRANCH_MISSES, context->GetField(kBranchMissesField));
    IncrementPerfCounter(thread, Q_BRANCH_LOADS, context->GetField(kBranchLoads));
    IncrementPerfCounter(thread, Q_BRANCH_LOAD_MISSES, context->GetField(kBranchLoadMissesField));
    IncrementPerfCounter(thread, Q_PAGE_FAULT, context->GetField(kPageFaultField));
    IncrementPerfCounter(thread, Q_FAULTS, context->GetField(kFaultsField));
    IncrementPerfCounter(thread, Q_MAJOR_FAULTS, context->GetField(kMajorFaultsField));
    IncrementPerfCounter(thread, Q_MINOR_FAULTS, context->GetField(kMinorFaultsField));
}

void LinuxPerfThreadMetricBlock::IncrementPerfCounter(
    uint32_t thread, quark::Quark counter, const value::Value* value)
{
    if (value == nullptr)
        return;

    uint64_t longValue = value->AsULong();
    auto look_last_value = _perfCounters[thread].find(counter);

    if (look_last_value != _perfCounters[thread].end())
    {        
        // Increment the counter for the thread.
        uint64_t increment = longValue - look_last_value->second;
        Builder()->IncrementProperty(thread, counter, increment);
    }

    // Remember the last value read for this counter.
    _perfCounters[thread][counter] = longValue;

    // TODO: Delete counter when a thread is terminated.
}

}  // namespace metric_blocks
}  // namespace tibee
