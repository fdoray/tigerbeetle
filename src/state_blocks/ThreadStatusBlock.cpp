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
#include "state_blocks/ThreadStatusBlock.hpp"

#include <boost/filesystem.hpp>
#include <fstream>
#include <time.h>

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Token.hpp"
#include "state_blocks/AttributeTreeWriter.hpp"
#include "state_blocks/QuarkDatabaseWriter.hpp"
#include "value/Utils.hpp"

namespace tibee
{
namespace state_blocks
{
using notification::Token;

ThreadStatusBlock::ThreadStatusBlock() :
    _currentState(nullptr)
{
    _filenamePrefix = std::to_string(time(NULL));
}

void ThreadStatusBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kThreadStatusServiceName, this);
}

void ThreadStatusBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(
        kCurrentStateServiceName,
        reinterpret_cast<void**>(&_currentState));
}

void ThreadStatusBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kCurrentStateNotificationPrefix)},
        base::BindObject(&ThreadStatusBlock::onStateChange, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kEndNotificationName)},
        base::BindObject(&ThreadStatusBlock::onEnd, this));
}

void ThreadStatusBlock::onStateChange(const notification::Path& path, const value::Value* value)
{
    // Keep track of threads status.
    if (path[2].token() == kStateThreads && path[4].token() == kStateStatus)
    {
        uint32_t attributeKey = value->GetField(kCurrentStateAttributeKeyField)->AsUInteger();
        const value::Value* attributeValue = _currentState->GetAttributeValue(state::AttributeKey(attributeKey));
        if (attributeValue == nullptr)
            return;
        quark::Quark attributeValueQuark(attributeValue->AsUInteger());

        ThreadStatus status = kUnknown;
        if (attributeValueQuark == _currentState->Quark(kStateRunUsermode))
            status = kUsermode;
        else if (attributeValueQuark == _currentState->Quark(kStateRunSyscall))
            status = kSyscall;
        else if (attributeValueQuark == _currentState->Quark(kStateInterrupted))
            status = kInterrupted;
        else if (attributeValueQuark == _currentState->Quark(kStateWaitBlocked))
            status = kWaitBlocked;
        else if (attributeValueQuark == _currentState->Quark(kStateWaitForCpu))
            status = kWaitCpu;
        else
            std::cout << "unknown status " << _currentState->String(attributeValueQuark) << std::endl;

        ThreadStatusInterval threadStatusInterval;
        threadStatusInterval.start = _currentState->GetAttributeLastChange(state::AttributeKey(attributeKey));
        threadStatusInterval.end = _currentState->timestamp();
        threadStatusInterval.status = status;

        uint32_t tid = atoi(path[3].token().c_str());
        _threadStatus[tid].push_back(threadStatusInterval);
    }
}

void ThreadStatusBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    namespace bfs = boost::filesystem;
    bfs::path historyDir(kStateHistoryDirectory);

    // Write thread status in a file.
    bfs::path threadStatusFilename = historyDir / (_filenamePrefix + kThreadStatusFilename);
    std::ofstream threadStatusFile;
    threadStatusFile.open(threadStatusFilename.string(), std::ios::out | std::ios::binary);

    // Write number of threads.
    uint32_t numThreads = _threadStatus.size();
    threadStatusFile.write(reinterpret_cast<const char*>(&numThreads), sizeof(numThreads));

    // Write status for each thread.
    for (const auto& threadIntervals : _threadStatus)
    {
        // Write tid.
        uint32_t tid = threadIntervals.first;
        threadStatusFile.write(reinterpret_cast<const char*>(&tid), sizeof(tid));

        // Write number of intervals.
        uint32_t numIntervals = threadIntervals.second.size();
        threadStatusFile.write(reinterpret_cast<const char*>(&numIntervals), sizeof(numIntervals));

        // Write intervals.
        for (const auto& interval : threadIntervals.second)
        {
            threadStatusFile.write(reinterpret_cast<const char*>(&interval), sizeof(interval));
        }
    }

    threadStatusFile.close();
}

}  // namespace state_blocks
}  // namespace tibee
