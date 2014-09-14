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
#include <boost/regex.hpp>

#include <common/traceplayback/EventHandlerSelector.hpp>

namespace tibee
{
namespace common
{
namespace
{

/**
 * Simple string matching function.
 *
 * Compares strings exactly and considers an empty \p asked string
 * as a wildcard.
 *
 * @param asked     Asked string
 * @param candidate Candidate to match
 * @returns         True if there's a match
 */
static bool namesMatchSimple(const std::string& asked,
                             const std::string& candidate) {
    return asked.empty() || asked == candidate;
}

}

EventHandlerSelector::EventHandlerSelector() :
    _traceSet {nullptr}
{
}

EventHandlerSelector::~EventHandlerSelector()
{
}

void EventHandlerSelector::setTraceSet(const TraceSet* traceSet)
{
    _traceSet = traceSet;
    _infamousMap.clear();
}

bool EventHandlerSelector::onEvent(const Event& event)
{
    // try finding a matching event callback function
    auto traceId = event.getTraceId();
    auto eventId = event.getId();

    auto callbackMapIt = _infamousMap.find(traceId);

    if (callbackMapIt != _infamousMap.end()) {
        const auto& callbackMap = callbackMapIt->second;

        auto callbackIt = callbackMap.find(eventId);

        if (callbackIt != callbackMap.end()) {
            const auto& callback = callbackIt->second;

            if (callback) {
                // match!
                return callback(event);
            }
        }
    }

    // no match: continue
    return true;
}

bool EventHandlerSelector::registerEventCallback(const std::string& traceType,
                                                 const std::string& eventName,
                                                 const OnEventFunc& onEvent)
{
    const auto& tracesInfos = _traceSet->getTracesInfos();

    bool matchLatch = false;

    for (const auto& traceInfos : tracesInfos) {
        if (namesMatchSimple(traceType, traceInfos->getTraceType())) {
            for (const auto& eventNameIdPair : *traceInfos->getEventMap()) {
                if (namesMatchSimple(eventName, eventNameIdPair.first)) {

                    auto traceId = traceInfos->getId();
                    auto eventId = eventNameIdPair.second->getId();

                    if (!_infamousMap[traceId][eventId]) {
                        _infamousMap[traceId][eventId] = onEvent;
                        matchLatch = true;
                    }
                }
            }
        }
    }

    return matchLatch;
}

bool EventHandlerSelector::registerEventCallbackRegex(const std::string& traceTypeRe,
                                                      const std::string& eventNameRe,
                                                      const OnEventFunc& onEvent)
{
    // try building/compiling regular expressions now
    boost::regex traceTypeBre;
    boost::regex eventNameBre;

    try {
        traceTypeBre = traceTypeRe;
        eventNameBre = eventNameRe;
    } catch (const std::exception& ex) {
        return false;
    }

    // find matches
    const auto& tracesInfos = _traceSet->getTracesInfos();

    bool matchLatch = false;

    for (const auto& traceInfos : tracesInfos) {
        if (boost::regex_search(traceInfos->getTraceType(), traceTypeBre)) {
            for (const auto& eventNameIdPair : *traceInfos->getEventMap()) {
                if (boost::regex_search(eventNameIdPair.first, eventNameBre)) {
                    auto traceId = traceInfos->getId();
                    auto eventId = eventNameIdPair.second->getId();

                    if (!_infamousMap[traceId][eventId]) {
                        _infamousMap[traceId][eventId] = onEvent;
                        matchLatch = true;
                    }
                }
            }
        }
    }

    return matchLatch;
}

}
}
