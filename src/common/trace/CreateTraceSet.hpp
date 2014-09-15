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
#ifndef _TIBEE_COMMON_CREATETRACESET_HPP
#define _TIBEE_COMMON_CREATETRACESET_HPP

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

#include <common/trace/TraceSet.hpp>

namespace tibee
{
namespace common
{

/**
 * Creates a trace set from a vector of trace paths.
 *
 * @param tracePaths       Vector of trace paths.
 * @returns The create trace set.
 */
std::unique_ptr<TraceSet> CreateTraceSet(
    const std::vector<boost::filesystem::path>& tracePaths);

}
}

#endif // _TIBEE_COMMON_CREATETRACESET_HPP
