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
#include "history/ThreadStatusReader.hpp"

#include <fstream>

namespace tibee
{
namespace history
{

bool ReadThreadStatus(const boost::filesystem::path& filename,
                      ThreadStatusMap* threadStatus)
{
    std::ifstream in(filename.string(), std::ios::in | std::ios::binary);

    // Read number of threads.
    uint32_t numThreads = 0;
    in.read(reinterpret_cast<char*>(&numThreads), sizeof(numThreads));

    // Read thread status.
    for (size_t i = 0; i < numThreads; ++i)
    {
        // Read tid.
        uint32_t tid = 0;
        in.read(reinterpret_cast<char*>(&tid), sizeof(tid));

        // Read number of intervals.
        uint32_t numIntervals = 0;
        in.read(reinterpret_cast<char*>(&numIntervals), sizeof(numIntervals));

        std::vector<ThreadStatusInterval> threadIntervals;

        // Read intervals.
        for (size_t j = 0; j < numIntervals; ++j)
        {
            ThreadStatusInterval interval;
            in.read(reinterpret_cast<char*>(&interval), sizeof(interval));

            threadIntervals.push_back(interval);
        }

        (*threadStatus)[tid] = threadIntervals;
    }

    return true;
}

}
}
