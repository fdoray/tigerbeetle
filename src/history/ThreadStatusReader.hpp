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
#ifndef _TIBEE_HISTORY_THREADSTATUSREADER_HPP
#define _TIBEE_HISTORY_THREADSTATUSREADER_HPP

#include <boost/filesystem.hpp>
#include <unordered_map>
#include <vector>

#include "base/BasicTypes.hpp"

namespace tibee
{
namespace history
{

enum ThreadStatus {
    kUsermode = 0,
    kSyscall,
    kInterrupted,
    kWaitCpu,
    kWaitBlocked,
    kUnknown,
};
struct ThreadStatusInterval {
    timestamp_t start;
    timestamp_t end;
    ThreadStatus status;
};

typedef std::unordered_map<uint32_t, std::vector<ThreadStatusInterval>> ThreadStatusMap;

bool ReadThreadStatus(const boost::filesystem::path& filename,
                      ThreadStatusMap* threadStatus);

}
}

#endif // _TIBEE_HISTORY_THREADSTATUSREADER_HPP
