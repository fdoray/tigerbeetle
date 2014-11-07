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
#include "builder_blocks/LinuxSyscallBuilderBlock.hpp"

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
namespace builder_blocks {

namespace
{
using notification::AnyToken;
using notification::Token;

const size_t kTidPathIndex = 3;
}  // namespace

LinuxSyscallBuilderBlock::LinuxSyscallBuilderBlock()
{
}

void LinuxSyscallBuilderBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kExecutionBuilderServiceName,
                             reinterpret_cast<void**>(&_executionBuilder));

    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    // Get constant quarks.
    Q_NODE_TYPE = _currentState->Quark(kNodeType);
}

void LinuxSyscallBuilderBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kCurrentStateNotificationPrefix), Token(kStateLinux), Token(kStateThreads), AnyToken(), Token(kStateSyscall)},
        base::BindObject(&LinuxSyscallBuilderBlock::onSyscall, this));
}

void LinuxSyscallBuilderBlock::onSyscall(const notification::Path& path, const value::Value* value)
{
    uint64_t tid = atoi(path[kTidPathIndex].token().c_str());
    
    auto syscallValue = value->GetField(kCurrentStateAttributeValueField);

    if (syscallValue != nullptr)
    {
        // Starting a syscall.
        if (_executionBuilder->PushStack(tid)) {
            _executionBuilder->SetProperty(tid, Q_NODE_TYPE, value::MakeValue(syscallValue->AsString()));
        }
    }
    else
    {
        // Ending a syscall.
        _executionBuilder->PopStack(tid);
    }
}

}  // namespace builder_blocks
}  // namespace tibee
