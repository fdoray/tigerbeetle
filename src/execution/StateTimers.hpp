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
#ifndef _TIBEE_EXECUTION_STATETIMERS_HPP
#define _TIBEE_EXECUTION_STATETIMERS_HPP

#include <functional>
#include <unordered_map>

#include "quark/Quark.hpp"

namespace tibee {
namespace execution {

class StateTimers
{
public:
    typedef std::function<void (
        quark::Quark state,
        uint64_t elapsed_time)> ReadTimerCallback;

    StateTimers();
    ~StateTimers();

    void StartTimer(uint64_t ts, quark::Quark state);
    bool ReadTimer(uint64_t ts,
                   quark::Quark state,
                   uint64_t* elapsed_time);
    bool ReadAndStopTimer(uint64_t ts,
                          quark::Quark state,
                          uint64_t* elapsed_time);
    void ReadAndResetTimers(uint64_t ts,
                            const ReadTimerCallback& callback);

private:
    // Active timers.
    typedef std::unordered_map<quark::Quark, uint64_t> Timers;
    Timers _timers;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STATETIMERS_HPP