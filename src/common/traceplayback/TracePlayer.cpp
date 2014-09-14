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
#include <memory>
#include <string>
#include <boost/filesystem/path.hpp>

#include <common/trace/TraceSet.hpp>
#include <common/trace/Event.hpp>
#include "TracePlayer.hpp"

namespace bfs = boost::filesystem;

namespace tibee
{
namespace common
{

TracePlayer::TracePlayer() :
    _playing {false}
{
}

bool TracePlayer::play(const common::TraceSet* traceSet,
                       const std::vector<AbstractTracePlaybackListener::UP>& listeners)
{
    // mark as playing
    _playing = true;

    // start
    for (auto& listener : listeners) {
        listener->onStart(traceSet);
    }

    // go through all events
    for (auto& event : *traceSet) {
        if (!_playing) {
            return false;
        }

        // play this event to all listeners
        for (auto& listener : listeners) {
            listener->onEvent(event);
        }
    }

    // stop
    for (auto& listener : listeners) {
        listener->onStop();
    }

    // not playing anymore
    _playing = false;

    return true;
}

void TracePlayer::stop()
{
    _playing = false;
}

}
}
