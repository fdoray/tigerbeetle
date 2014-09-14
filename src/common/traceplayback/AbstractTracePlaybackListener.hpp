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
#ifndef _TIBEE_COMMON_ABSTRACTTRACEPLAYBACKLISTENER_HPP
#define _TIBEE_COMMON_ABSTRACTTRACEPLAYBACKLISTENER_HPP

#include <memory>

#include <common/trace/TraceSet.hpp>
#include <common/trace/Event.hpp>

namespace tibee
{
namespace common
{

/**
 * Abstract trace playback listener (observer).
 *
 * This is a simple interface which gets notified when beginning the
 * playback of a trace, on each event, and at the end.
 *
 * @author Philippe Proulx
 */
class AbstractTracePlaybackListener
{
public:
    /// Unique pointer to track playback listener
    typedef std::unique_ptr<AbstractTracePlaybackListener> UP;

public:
    virtual ~AbstractTracePlaybackListener() = 0;

    /**
     * Playback start notification.
     *
     * The trace set pointer passed as parameter \p traceSet remains
     * valid until onStop() is called and returns.
     *
     * @param traceSet Trace set which will be used for reading
     * @returns        True if the initialization was successful
     */
    bool onStart(const common::TraceSet* traceSet)
    {
        return this->onStartImpl(traceSet);
    }

    /**
     * New event notification.
     *
     * @param event New event
     */
    void onEvent(const common::Event& event)
    {
        this->onEventImpl(event);
    }

    /**
     * Playback stop notification.
     *
     * @returns True if the finalization was successful
     */
    bool onStop()
    {
        return this->onStopImpl();
    }

private:
    virtual bool onStartImpl(const common::TraceSet* traceSet) = 0;
    virtual void onEventImpl(const common::Event& event) = 0;
    virtual bool onStopImpl() = 0;
};

}
}

#endif // _TIBEE_COMMON_ABSTRACTTRACEPLAYBACKLISTENER_HPP
