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
#include "analysis_blocks/LinuxPerfBuilderBlock.hpp"

#include <stdlib.h>

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "trace/value/EventValue.hpp"
#include "value/MakeValue.hpp"
#include "value/ReadValue.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace analysis_blocks {

namespace
{
using notification::Token;
using state_blocks::CurrentStateBlock;

const char kInstructionsField[] = "perf_cpu_instructions";
const char kCacheReferencesField[] = "perf_cpu_cache_references";
const char kCacheMissesField[] = "perf_cpu_cache_misses";
const char kBranchInstructionsField[] = "perf_cpu_branch_instructions";
const char kBranchesField[] = "perf_cpu_branches";
const char kBranchMissesField[] = "perf_cpu_branch_misses";
const char kBranchLoadsField[] = "perf_cpu_branch_loads";
const char kBranchLoadMissesField[] = "perf_cpu_branch_load_misses";
const char kPageFaultField[] = "perf_cpu_page_fault";
const char kFaultsField[] = "perf_cpu_faults";
const char kMajorFaultsField[] = "perf_cpu_major_faults";
const char kMinorFaultsField[] = "perf_cpu_minor_faults";

}  // namespace

LinuxPerfBuilderBlock::LinuxPerfBuilderBlock()
{
}

void LinuxPerfBuilderBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kGraphBuilderServiceName,
                             reinterpret_cast<void**>(&_graphBuilder));

    serviceList.QueryService(CurrentStateBlock::kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    // Get constant quarks.
    Q_INSTRUCTIONS = _currentState->Quark(kInstructions);
    Q_CACHE_REFERENCES = _currentState->Quark(kCacheReferences);
    Q_CACHE_MISSES = _currentState->Quark(kCacheMisses);
    Q_BRANCH_INSTRUCTIONS = _currentState->Quark(kBranchInstructions);
    Q_BRANCHES = _currentState->Quark(kBranches);
    Q_BRANCH_MISSES = _currentState->Quark(kBranchMisses);
    Q_BRANCH_LOADS = _currentState->Quark(kBranchLoads);
    Q_BRANCH_LOAD_MISSES = _currentState->Quark(kBranchLoadMisses);
    Q_PAGE_FAULT = _currentState->Quark(kPageFault);
    Q_FAULTS = _currentState->Quark(kFaults);
    Q_MAJOR_FAULTS = _currentState->Quark(kMajorFaults);
    Q_MINOR_FAULTS = _currentState->Quark(kMinorFaults);

    Q_CUR_THREAD = _currentState->Quark(kStateCurThread);

    // CPUs attribute.
    _cpusAttribute = _currentState->GetAttributeKey({
        _currentState->Quark(kStateLinux),
        _currentState->Quark(kStateCpus)
    });
}

void LinuxPerfBuilderBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token("lttng-kernel")},
        base::BindObject(&LinuxPerfBuilderBlock::onEvent, this));
}

void LinuxPerfBuilderBlock::onEvent(const notification::Path& path, const value::Value* value)
{
    auto event = reinterpret_cast<const trace::EventValue*>(value);
    auto cpu = event->getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
    auto context = value::StructValueBase::Cast(event->getStreamEventContext());

    auto threadValue = _currentState->GetAttributeValue(
        _cpusAttribute,
        {_currentState->IntQuark(cpu), Q_CUR_THREAD});
    if (threadValue == nullptr)
        return;
    auto thread = threadValue->AsInteger();

    IncrementPerfCounter(cpu, thread, Q_INSTRUCTIONS, context->GetField(kInstructionsField));
    IncrementPerfCounter(cpu, thread, Q_CACHE_REFERENCES, context->GetField(kCacheReferencesField));
    IncrementPerfCounter(cpu, thread, Q_CACHE_MISSES, context->GetField(kCacheMissesField));
    IncrementPerfCounter(cpu, thread, Q_BRANCH_INSTRUCTIONS, context->GetField(kBranchInstructionsField));
    IncrementPerfCounter(cpu, thread, Q_BRANCHES, context->GetField(kBranchesField));
    IncrementPerfCounter(cpu, thread, Q_BRANCH_MISSES, context->GetField(kBranchMissesField));
    IncrementPerfCounter(cpu, thread, Q_BRANCH_LOADS, context->GetField(kBranchLoads));
    IncrementPerfCounter(cpu, thread, Q_BRANCH_LOAD_MISSES, context->GetField(kBranchLoadMissesField));
    IncrementPerfCounter(cpu, thread, Q_PAGE_FAULT, context->GetField(kPageFaultField));
    IncrementPerfCounter(cpu, thread, Q_FAULTS, context->GetField(kFaultsField));
    IncrementPerfCounter(cpu, thread, Q_MAJOR_FAULTS, context->GetField(kMajorFaultsField));
    IncrementPerfCounter(cpu, thread, Q_MINOR_FAULTS, context->GetField(kMinorFaultsField));
}

void LinuxPerfBuilderBlock::IncrementPerfCounter(
    uint32_t cpu, int32_t thread, quark::Quark counter, const value::Value* value)
{
    if (value == nullptr)
        return;

    if (_perfCounters.size() < cpu + 1)
        _perfCounters.resize(cpu + 1);

    uint64_t longValue = value->AsULong();
    auto look_last_value = _perfCounters[cpu].find(counter);

    if (look_last_value != _perfCounters[cpu].end())
    {
        // Increment the counter for the thread.
        uint64_t increment = longValue - look_last_value->second;

        _graphBuilder->IncrementProperty(thread, counter, increment);
    }

    // Remember the last value read for this counter.
    _perfCounters[cpu][counter] = longValue;
}

}  // namespace analysis_blocks
}  // namespace tibee
