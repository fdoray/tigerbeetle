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
#ifndef _ARGUMENTS_HPP
#define _ARGUMENTS_HPP

#include <vector>
#include <string>

namespace tibee
{

/**
 * Program arguments.
 *
 * @author Francois Doray
 */
struct Arguments
{
    std::vector<std::string> trace_a;
    std::vector<std::string> trace_b;
    std::vector<std::string> stateProviders;
    std::vector<std::string> stateProvidersParams;
    bool verbose;
};

}

#endif // _ARGUMENTS_HPP
