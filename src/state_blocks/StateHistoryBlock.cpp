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
#include "state_blocks/StateHistoryBlock.hpp"

#include <delorean/interval/Int32Interval.hpp>
#include <delorean/interval/Uint32Interval.hpp>
#include <delorean/interval/Int64Interval.hpp>
#include <delorean/interval/Uint64Interval.hpp>
#include <delorean/interval/Float32Interval.hpp>
#include <delorean/interval/QuarkInterval.hpp>
#include <delorean/interval/NullInterval.hpp>
#include <fstream>
#include <iostream>
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

StateHistoryBlock::StateHistoryBlock() :
    _currentState(nullptr),
    _intervalFileSink(new delo::HistoryFileSink)
{
    namespace bfs = boost::filesystem;
    bfs::path historyDir(kStateHistoryDirectory);

    _filenamePrefix = std::to_string(time(NULL));
    bfs::path intervalFile = historyDir / (_filenamePrefix + kHistoryTreeFilename);
    _intervalFileSink->open(intervalFile.string());

    InitTranslators();
}

void StateHistoryBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kHistoryServiceName, this);
}

void StateHistoryBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(
        kCurrentStateServiceName,
        reinterpret_cast<void**>(&_currentState));
}

void StateHistoryBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kCurrentStateNotificationPrefix)},
        base::BindObject(&StateHistoryBlock::onStateChange, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kEndNotificationName)},
        base::BindObject(&StateHistoryBlock::onEnd, this));
}

void StateHistoryBlock::InitTranslators()
{
    _nullTranslator = [this] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        return new delo::NullInterval {
            static_cast<delo::timestamp_t>(start),
            static_cast<delo::timestamp_t>(end),
            static_cast<delo::interval_key_t>(key)
        };
    };

    auto unknownTranslator = [] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        return nullptr;
    };

    auto sint32Translator = [this] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        auto interval = new delo::Int32Interval {
            static_cast<delo::timestamp_t>(start),
            static_cast<delo::timestamp_t>(end),
            static_cast<delo::interval_key_t>(key)
        };

        interval->setValue(value->AsInteger());

        return interval;
    };

    auto uint32Translator = [this] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        auto interval = new delo::Uint32Interval {
            static_cast<delo::timestamp_t>(start),
            static_cast<delo::timestamp_t>(end),
            static_cast<delo::interval_key_t>(key)
        };

        interval->setValue(value->AsUInteger());

        return interval;
    };

    auto sint64Translator = [this] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        auto interval = new delo::Int64Interval {
            static_cast<delo::timestamp_t>(start),
            static_cast<delo::timestamp_t>(end),
            static_cast<delo::interval_key_t>(key)
        };

        interval->setValue(value->AsLong());

        return interval;
    };

    auto uint64Translator = [this] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        auto interval = new delo::Uint64Interval {
            static_cast<delo::timestamp_t>(start),
            static_cast<delo::timestamp_t>(end),
            static_cast<delo::interval_key_t>(key)
        };

        interval->setValue(value->AsULong());

        return interval;
    };

    auto floatTranslator = [this] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        auto interval = new delo::Float32Interval {
            static_cast<delo::timestamp_t>(start),
            static_cast<delo::timestamp_t>(end),
            static_cast<delo::interval_key_t>(key)
        };

        interval->setValue(value->AsFloating());

        return interval;
    };

    auto stringTranslator = [this] (uint32_t key, const value::Value* value, timestamp_t start, timestamp_t end)
    {
        auto interval = new delo::QuarkInterval {
            static_cast<delo::timestamp_t>(start),
            static_cast<delo::timestamp_t>(end),
            static_cast<delo::interval_key_t>(key)
        };

        interval->setValue(static_cast<uint32_t>(_currentState->Quark(value->AsString()).get()));

        return interval;
    };

    // fill translators
    for (auto& translator : _translators) {
        translator = unknownTranslator;
    }

    _translators[static_cast<std::size_t>(value::VALUE_BOOL)] = uint32Translator;
    _translators[static_cast<std::size_t>(value::VALUE_CHAR)] = sint32Translator;
    _translators[static_cast<std::size_t>(value::VALUE_UCHAR)] = uint32Translator;
    _translators[static_cast<std::size_t>(value::VALUE_SHORT)] = sint32Translator;
    _translators[static_cast<std::size_t>(value::VALUE_USHORT)] = uint32Translator;
    _translators[static_cast<std::size_t>(value::VALUE_INT)] = sint32Translator;
    _translators[static_cast<std::size_t>(value::VALUE_UINT)] = uint32Translator;
    _translators[static_cast<std::size_t>(value::VALUE_LONG)] = sint64Translator;
    _translators[static_cast<std::size_t>(value::VALUE_ULONG)] = uint64Translator;
    _translators[static_cast<std::size_t>(value::VALUE_FLOAT)] = floatTranslator;
    _translators[static_cast<std::size_t>(value::VALUE_DOUBLE)] = floatTranslator;
    _translators[static_cast<std::size_t>(value::VALUE_STRING)] = stringTranslator;
    _translators[static_cast<std::size_t>(value::VALUE_WSTRING)] = stringTranslator;
}

void StateHistoryBlock::onStateChange(const notification::Path& path, const value::Value* value)
{
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

    /*
    uint32_t attributeKey = value->GetField(kCurrentStateAttributeKeyField)->AsUInteger();
    const value::Value* attributeValue = value->GetField(kCurrentStateAttributeValueField);
    timestamp_t start = _currentState->GetAttributeLastChange(state::AttributeKey(attributeKey));
    timestamp_t end = _currentState->timestamp();

    delo::AbstractInterval* interval = nullptr;
    if (attributeValue == nullptr)
        interval = _nullTranslator(attributeKey, attributeValue, start, end);
    else
        interval = _translators[static_cast<size_t>(attributeValue->GetType())](attributeKey, attributeValue, start, end);

    _intervalFileSink->addInterval(delo::AbstractInterval::UP {interval});
    */
}

void StateHistoryBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    namespace bfs = boost::filesystem;
    bfs::path historyDir(kStateHistoryDirectory);

    // Close all intervals.
    _currentState->NullAttribute(keyed_tree::kRootNodeKey);

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

    /*
    // Write all remaining state values as intervals and close the interval tree file.
    _currentState->NullAttribute(keyed_tree::kRootNodeKey);
    _intervalFileSink->close();

    // Write the quark database.
    bfs::path quarkDatabaseFile = historyDir / (_filenamePrefix + kQuarkDatabaseFilename);
    WriteQuarkDatabase(quarkDatabaseFile.string(), _currentState->GetQuarks());

    // Write the attribute tree.
    bfs::path attributeTreeFile = historyDir / (_filenamePrefix + kAttributeTreeFilename);
    WriteAttributeTree(attributeTreeFile.string(), _currentState);
    */
}

}  // namespace state_blocks
}  // namespace tibee
