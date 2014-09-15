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
#ifndef _BUILDERBEETLE_HPP
#define _BUILDERBEETLE_HPP

#include <boost/filesystem/path.hpp>

#include <common/stateprov/StateProviderConfig.hpp>
#include <common/traceplayback/TracePlayer.hpp>
#include "StateHistoryBuilder.hpp"
#include "Arguments.hpp"

namespace tibee
{

/**
 * Builder beetle. This little beetle will build whatever you ask it.
 *
 * @author Philippe Proulx
 */
class BuilderBeetle
{
public:
    /**
     * Instanciates a builder beetle.
     *
     * @param args Program arguments
     */
    BuilderBeetle(const Arguments& args);

    /**
     * Runs the builder.
     *
     * @returns True if everything went fine
     */
    bool run();

    /**
     * Stops the builder.
     */
    void stop();

private:
    void validateSaveArguments(const Arguments& args);
    void createTracesSymlinks() const;

private:
    tibee::common::TracePlayer _tracePlayer;
    std::vector<boost::filesystem::path> _tracesPaths;
    std::vector<common::StateProviderConfig> _stateProviders;
    std::string _bindProgress;
    boost::filesystem::path _dbDir;
    bool _verbose;
};

}

#endif // _BUILDERBEETLE_HPP
