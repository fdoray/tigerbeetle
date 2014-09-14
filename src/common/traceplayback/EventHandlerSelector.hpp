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
#ifndef _TIBEE_COMMON_EVENTHANDLERSELECTOR_HPP
#define _TIBEE_COMMON_EVENTHANDLERSELECTOR_HPP

#include <boost/utility.hpp>
#include <functional>
#include <unordered_map>

#include <common/trace/Event.hpp>
#include <common/trace/TraceSet.hpp>

namespace tibee
{
namespace common
{

/**
 * Receives events and forwards them to the appropriate handler.
 *
 * @author Francois Doray
 */
class EventHandlerSelector :
    boost::noncopyable
{
public:
    /// On event function
    typedef std::function<bool (const Event& event)> OnEventFunc;

public:
    EventHandlerSelector();
    ~EventHandlerSelector();

    void setTraceSet(const TraceSet* traceSet);

    /**
     * Called on each event.
     *
     * @param event New event
     * @returns     True to continue
     */
    bool onEvent(const Event& event);

    /**
     * Registers an event callback to be called when an event matches
     * the specified (trace type, event name) pair.
     *
     * LTTng kernel traces have the trace type "lttng-kernel". LTTng
     * UST traces have the trace type "lttng-ust" and their event names
     * contain the UST provider name, followed by ":", followed by the
     * tracepoint name, e.g. "my_application:my_tracepoint_name".
     *
     * An empty trace type means "match all trace types not matched by
     * any other callback registered so far for the same event name".
     *
     * An empty event name means "match all events not matched by any
     * other callback registered so far for the same trace type".
     *
     * If both \p traceType and \p eventName are empty, the callback
     * is registered for all (trace type, event name) pairs which do
     * not have any registered callback so far.
     *
     * Be aware that calling this method shall _not_ override anything
     * previously set that matches the same constraints.
     *
     * The method returns true if there was at least one match for the
     * specified constraints.
     *
     * @param traceType Trace type
     * @param eventName Event name (empty string to match all)
     * @param onEvent   Callback function to be called during state construction
     * @returns         True if at least one event matched the constraints
     */
    bool registerEventCallback(const std::string& traceType,
                               const std::string& eventName,
                               const OnEventFunc& onEvent);

    /**
     * Registers an event callback to be called when an event matches
     * the specified (trace type regex, event name regex) pair using PCRE.
     *
     * Just like registerEventCallback(), but using PCRE for both
     * \p traceTypeRe and \p eventNameRe.
     *
     * The registerEventCallback() registration order applies: once
     * a callback is registered, it won't be overwritten, so make sure
     * to register the "catch all" (fallback) callbacks after the more
     * specific ones.
     *
     * Example:
     *
     *     ^sys_.*     -> onSysEvent
     *     ^sys_close$ -> onSysClose
     *
     * The `onSysClose` function will never be called since the
     * `onSysEvent` was registered before and matches `sys_close` too.
     *
     * The method returns false if there were no matches or if at least
     * one of the provided regular expressions was invalid.
     *
     * @see registerEventCallback()
     *
     * @param traceTypeRe Trace type PCRE
     * @param eventNameRe Event name PCRE
     * @param onEvent     Callback function to be called during state construction
     * @returns           True if at least one event matched the constraints
     */
    bool registerEventCallbackRegex(const std::string& traceTypeRe,
                                    const std::string& eventNameRe,
                                    const OnEventFunc& onEvent);

private:
    // (event ID -> event callback) map
    typedef std::unordered_map<event_id_t, OnEventFunc> EventIdCallbackMap;

    // (trace ID -> (event ID -> event callback)) map
    typedef std::unordered_map<trace_id_t, EventIdCallbackMap> TraceIdEventIdCallbackMap;

private:
    // master event callback map for this state provider
    TraceIdEventIdCallbackMap _infamousMap;

    // trace set.
    const TraceSet* _traceSet;
};

}
}

#endif // _TIBEE_COMMON_EVENTHANDLERSELECTOR_HPP
