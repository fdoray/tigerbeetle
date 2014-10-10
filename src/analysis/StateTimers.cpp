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
#include "analysis/StateTimers.hpp"

namespace tibee {
namespace analysis {

StateTimers::StateTimers() {
}

StateTimers::~StateTimers() {
    
}

void StateTimers::StartTimer(uint64_t ts, quark::Quark state) {
    if (_timers.find(state) != _timers.end())
        return;

    _timers[state] = ts;
}

bool StateTimers::ReadTimer(uint64_t ts,
                            quark::Quark state,
                            uint64_t* elapsed_time) {
    auto look = _timers.find(state);
    if (look == _timers.end())
        return false;

    *elapsed_time = ts - look->second;

    return true;
}

bool StateTimers::ReadAndStopTimer(uint64_t ts,
                                   quark::Quark state,
                                   uint64_t* elapsed_time) {
    auto look = _timers.find(state);
    if (look == _timers.end())
        return false;

    *elapsed_time = ts - look->second;
    _timers.erase(look);

    return true;
}

void StateTimers::ReadAndResetTimers(uint64_t ts,
                                     const ReadTimerCallback& callback) {
    for (auto& timer : _timers) {
        uint64_t elapsed_time = ts - timer.second;
        if (elapsed_time != 0)
        {
            callback(timer.first, elapsed_time);
        }
        timer.second = ts;
    }
}

}  // namespace analysis
}  // namespace tibee
