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
#ifndef _TIBEE_COMMON_TRACEPLAYER_HPP
#define _TIBEE_COMMON_TRACEPLAYER_HPP

#include <memory>
#include <string>
#include <boost/filesystem/path.hpp>

#include <common/trace/TraceSet.hpp>
#include <common/trace/Event.hpp>
#include "AbstractTracePlaybackListener.hpp"

namespace tibee
{
namespace common
{

/**
 * Trace player. Plays a trace set to one or more listeners.
 *
 * @author Philippe Proulx
 */
class TracePlayer
{
public:
    /**
     * Builds a trace player.
     */
    TracePlayer();

    /**
     * Starts playing the trace set \p traceSet from the beginning to
     * all listeners \p in listeners.
     *
     * @param traceSet  Trace set to play
     * @param listeners Listeners which will listen to the trace
     * @returns         True if the trace was played without interruption
     */
    bool play(const common::TraceSet* traceSet,
              const std::vector<AbstractTracePlaybackListener::UP>& listeners);

    /**
     * Stops any current playback.
     */
    void stop();

private:
    bool _playing;
};

}
}

#endif // _TIBEE_COMMON_TRACEPLAYER_HPP
