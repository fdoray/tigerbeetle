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
#ifndef _GRAPHBUILDER_TASKTIMERS_HPP
#define _GRAPHBUILDER_TASKTIMERS_HPP

#include <functional>
#include <unordered_map>

namespace tibee
{

class TaskTimers
{
public:
    typedef std::function<void (
        const std::string& timer_name,
        uint64_t elapsed_time)> ReadTimerCallback;

    TaskTimers();
    ~TaskTimers();

    void StartTimer(uint64_t ts, const std::string& timer_name);
    bool ReadAndStopTimer(uint64_t ts,
                          const std::string& timer_name,
                          uint64_t* elapsed_time);
    void ReadAndResetTimers(uint64_t ts,
                            const ReadTimerCallback& callback);

private:
    // Active timers.
    typedef std::unordered_map<std::string, uint64_t> Timers;
    Timers _timers;
};

}  // namespace tibee

#endif // _GRAPHBUILDER_TASKTIMERS_HPP
