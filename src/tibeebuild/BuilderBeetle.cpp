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
#include <set>
#include <vector>
#include <sstream>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

#include <common/trace/TraceSet.hpp>
#include <common/traceplayback/TracePlayer.hpp>
#include <common/stateprov/StateProviderConfig.hpp>
#include <common/utils/print.hpp>
#include <common/ex/WrongStateProvider.hpp>
#include "StateHistoryBuilder.hpp"
#include "ProgressPublisher.hpp"
#include "Arguments.hpp"
#include "BuilderBeetle.hpp"
#include "ex/MqBindError.hpp"
#include "ex/UnknownStateProviderType.hpp"
#include "ex/InvalidArgument.hpp"
#include "ex/BuilderBeetleError.hpp"
#include "ex/StateProviderNotFound.hpp"

#define THIS_MODULE "builder"

namespace bfs = boost::filesystem;

namespace tibee
{

using common::tbmsg;
using common::tbendl;

BuilderBeetle::BuilderBeetle(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void BuilderBeetle::validateSaveArguments(const Arguments& args)
{
    // make sure all traces actually exist and create paths
    for (const auto& pathStr : args.traces) {
        auto tracePath = bfs::path {pathStr};

        // make sure this trace exists (at least, may still be invalid)
        if (!bfs::exists(tracePath)) {
            std::stringstream ss;

            ss << "trace " << tracePath << " does not exist";

            throw ex::InvalidArgument {ss.str()};
        }

        // append trace path
        _tracesPaths.push_back(tracePath);
    }

    // create default database output directory if not specified
    if (args.dbDir.empty()) {
        _dbDir = bfs::current_path() / "tibee";
    } else {
        _dbDir = args.dbDir;
    }

    // make sure the database directory doesn't exist, unless force is enabled
    if (!args.force && bfs::exists(_dbDir)) {
        std::stringstream ss;

        ss << "the specified database directory " <<
              _dbDir << " exists already" << std::endl <<
              "  (use -f to overwrite files)";

        throw ex::InvalidArgument {ss.str()};
    } else if (args.force && bfs::exists(_dbDir) && !bfs::is_directory(_dbDir)) {
        std::stringstream ss;

        ss << "the specified database directory " <<
              _dbDir << " exists and is not a directory";

        throw ex::InvalidArgument {ss.str()};
    }

    // create specified directory now
    bfs::create_directories(_dbDir);

    // extract instance names from state provider names and keep them
    boost::regex spRe {"([A-Za-z0-9_][A-Za-z0-9_-]*):(.+)"};

    for (const auto& fullStateProvider : args.stateProviders) {
        boost::smatch m;
        std::string instance;
        std::string name;

        // has instance name?
        auto hasInstance = boost::regex_match(fullStateProvider, m, spRe);

        if (hasInstance) {
            instance = m[1];
            name = m[2];
        } else {
            name = fullStateProvider;
        }

        _stateProviders.push_back({
            name,
            instance
        });
    }

    // make sure all state provider instance names are unique
    std::set<std::string> set;

    for (const auto& stateProviderConfig : _stateProviders) {
        const auto& instance = stateProviderConfig.getInstanceName();

        if (!instance.empty()) {
            if (set.find(instance) != set.end()) {
                std::stringstream ss;

                ss << "duplicate state provider instance name: \"" <<
                      instance << "\"";

                throw ex::InvalidArgument {ss.str()};
            }

            set.insert(instance);
        }
    }

    // evaluate state providers parameters

    // empty values are valid, hence (.*)
    boost::regex gpRe {"([A-Za-z0-9_][A-Za-z0-9_-]*)=(.*)"};
    boost::regex inspRe {"([A-Za-z0-9_][A-Za-z0-9_-]*):([A-Za-z0-9_][A-Za-z0-9_-]*)=(.*)"};

    for (const auto& fullParam : args.stateProvidersParams) {
        boost::smatch m;

        if (boost::regex_match(fullParam, m, gpRe)) {
            // global
            for (auto& stateProviderConfig : _stateProviders) {
                auto& params = stateProviderConfig.getParams();

                params[m[1]] = common::StateProviderParamValue {m[2]};
            }
        } else if (boost::regex_match(fullParam, m, inspRe)) {
            // specific to instance
            bool found = false;

            // omg, linear search: we should survive
            for (auto& stateProviderConfig : _stateProviders) {
                if (stateProviderConfig.getInstanceName() == m[1]) {
                    auto& params = stateProviderConfig.getParams();

                    params[m[2]] = common::StateProviderParamValue {m[3]};
                    found = true;
                    break;
                }
            }

            if (!found) {
                std::stringstream ss;

                ss << "cannot find state provider instance \"" <<
                      m[1] << "\" for parameter \"" <<
                      fullParam << "\"";

                throw ex::InvalidArgument {ss.str()};
            }
        } else {
            // wrong format
            std::stringstream ss;

            ss << "wrong state provider parameter format: \"" <<
                  fullParam << "\"";

            throw ex::InvalidArgument {ss.str()};
        }
    }

    // bind address for progress publishing
    _bindProgress = args.bindProgress;

    // verbose
    _verbose = args.verbose;
}

void BuilderBeetle::createTracesSymlinks() const
{
    auto tracesSymlinksDir = _dbDir / "traces";

    // create "traces" subdirectory of database directory
    try {
        bfs::create_directory(tracesSymlinksDir);
    } catch (const std::exception& ex) {
        std::stringstream ss;

        ss << "cannot create " << tracesSymlinksDir << " directory" <<
              "  " << ex.what();

        throw ex::BuilderBeetleError {ss.str()};
    }

    // make sure the directory exists
    if (!bfs::is_directory(tracesSymlinksDir)) {
        std::stringstream ss;

        ss << "cannot create " << tracesSymlinksDir << " directory";

        throw ex::BuilderBeetleError {ss.str()};
    }

    // create symlinks
    std::size_t cur = 0;

    for (const auto& tracePath : _tracesPaths) {
        auto symlinkPath = tracesSymlinksDir / std::to_string(cur);

        if (bfs::exists(symlinkPath)) {
            try {
                bfs::remove(symlinkPath);
            } catch (const std::exception& ex) {
                std::stringstream ss;

                ss << "cannot remove existing file " << symlinkPath <<
                      "  " << ex.what();

                throw ex::BuilderBeetleError {ss.str()};
            }
        }

        try {
            bfs::create_symlink(tracePath, symlinkPath);
        } catch (const std::exception& ex) {
            std::stringstream ss;

            ss << "cannot create symlink " << symlinkPath <<
                  "  " << ex.what();

            throw ex::BuilderBeetleError {ss.str()};
        }
    }
}

bool BuilderBeetle::run()
{
    if (_verbose) {
        tbmsg(THIS_MODULE) << "starting builder" << tbendl();
    }

    // create traces symlinks
    this->createTracesSymlinks();

    // create a trace set
    std::unique_ptr<common::TraceSet> traceSet {new common::TraceSet};

    // add traces to trace set
    for (const auto& tracePath : _tracesPaths) {
        if (_verbose) {
            tbmsg(THIS_MODULE) << "adding trace " << tracePath << tbendl();
        }

        if (!traceSet->addTrace(tracePath)) {
            std::stringstream ss;

            ss << "could not add trace " << tracePath << " (internal error)";

            throw ex::BuilderBeetleError {ss.str()};
        }
    }

    // create a list of trace listeners
    std::vector<tibee::common::AbstractTracePlaybackListener::UP> listeners;

    // create a state history builder (if we have at least one provider)
    std::unique_ptr<StateHistoryBuilder> stateHistoryBuilder;

    // this if for the progress publisher
    const StateHistoryBuilder* shbPtr = nullptr;

    if (!_stateProviders.empty()) {
        try {
            stateHistoryBuilder = std::unique_ptr<StateHistoryBuilder> {
                new StateHistoryBuilder {
                    _dbDir,
                    _stateProviders
                }
            };
        } catch (const common::ex::WrongStateProvider& ex) {
            std::stringstream ss;

            ss << "wrong state provider: \"" << ex.getName() << "\"" << std::endl <<
                  "  " << ex.what();

            throw ex::BuilderBeetleError {ss.str()};
        } catch (const ex::UnknownStateProviderType& ex) {
            std::stringstream ss;

            ss << "unknown state provider type: \"" << ex.getName() << "\"";

            throw ex::BuilderBeetleError {ss.str()};
        } catch (const ex::StateProviderNotFound& ex) {
            std::stringstream ss;

            ss << "cannot find state provider \"" << ex.getName() << "\"";

            throw ex::BuilderBeetleError {ss.str()};
        } catch (...) {
            throw ex::BuilderBeetleError {"unknown error"};
        }

        // reference for progress publisher before moving it
        shbPtr = stateHistoryBuilder.get();

        listeners.push_back(std::move(stateHistoryBuilder));
    }

    // create a progress publisher
    if (!_bindProgress.empty()) {
        std::unique_ptr<ProgressPublisher> progressPublisher;

        try {
            progressPublisher = std::unique_ptr<ProgressPublisher> {
                new ProgressPublisher {
                    _bindProgress,
                    traceSet->getBegin(),
                    traceSet->getEnd(),
                    _tracesPaths,
                    _stateProviders,
                    shbPtr,
                    2801,
                    250
                }
            };
        } catch (const ex::MqBindError& ex) {
            std::stringstream ss;

            ss << "cannot bind to address \"" << ex.getBindAddr() << "\"";

            throw ex::BuilderBeetleError {ss.str()};
        }

        listeners.push_back(std::move(progressPublisher));
    }

    // ready for playback!
    if (_verbose) {
        tbmsg(THIS_MODULE) << "starting trace playback" << tbendl();
    }

    return _tracePlayer.play(traceSet.get(), listeners);
}

void BuilderBeetle::stop()
{
    _tracePlayer.stop();
}

}
