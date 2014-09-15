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
#ifndef _TIBEECOMPARE_HPP
#define _TIBEECOMPARE_HPP

#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

#include "Arguments.hpp"

namespace tibee
{

/**
 * Compares two traces.
 *
 * @author Francois Doray
 */
class TibeeCompare
{
public:
    /**
     * Instanciates a TibeeCompare program.
     *
     * @param args Program arguments
     */
    TibeeCompare(const Arguments& args);

    /**
     * Runs the comparison.
     *
     * @returns True if everything went fine
     */
    bool run();

private:
    void validateSaveArguments(const Arguments& args);
    void validateSaveTracePaths(const std::vector<std::string>& pathsStr,
                                std::vector<boost::filesystem::path>* paths);

private:
    std::vector<boost::filesystem::path> _traceAPaths;
    std::vector<boost::filesystem::path> _traceBPaths;
    bool _verbose;
};

}

#endif // _TIBEECOMPARE_HPP
