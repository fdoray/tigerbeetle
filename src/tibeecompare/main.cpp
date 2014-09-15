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
#include <iostream>
#include <boost/program_options.hpp>

#include <common/utils/print.hpp>
#include "Arguments.hpp"
#include "TibeeCompare.hpp"
#include "ex/InvalidArgument.hpp"


using tibee::common::tberror;
using tibee::common::tbendl;

namespace
{

/**
 * Parses the command line arguments passed to the program.
 *
 * @param argc Number of arguments in \p argv
 * @param argv Command line arguments
 * @param args Arguments values to fill
 *
 * @returns    0 to continue, 1 if there's a command line error
 */
int parseOptions(int argc, char* argv[], tibee::Arguments& args)
{
    namespace bpo = boost::program_options;

    bpo::options_description desc;

    desc.add_options()
        ("help,h", "help")
        ("tracea,a", bpo::value<std::vector<std::string>>())
        ("traceb,b", bpo::value<std::vector<std::string>>())
        ("verbose,v", bpo::bool_switch()->default_value(false))
        ("stateprov,s", bpo::value<std::vector<std::string>>())
        ("param,p", bpo::value<std::vector<std::string>>())
    ;

    bpo::variables_map vm;

    try {
        auto cliParser = bpo::command_line_parser(argc, argv);
        auto parsedOptions = cliParser.options(desc).run();

        bpo::store(parsedOptions, vm);
    } catch (const std::exception& ex) {
        tberror() << "command line error: " << ex.what() << tbendl();
        return 1;
    }

    if (!vm["help"].empty()) {
        std::cout <<
            "usage: " << argv[0] << " [options]" << std::endl <<
            std::endl <<
            "options:" << std::endl <<
            std::endl <<
            "  -h, --help                  print this help message" << std::endl <<
            "  -a                          path of the first trace" << std::endl <<
            "  -b                          path of the second trace" << std::endl <<
            "  -p [<inst>:]<key>=<val>     state provider parameter" << std::endl <<
            "  -s [<inst>:]<name>          state provider name with optional unique" << std::endl <<
            "                              instance name <inst>; <name> may be a path" << std::endl <<
            "  -v, --verbose               verbose" << std::endl;

        return -1;
    }

    try {
        vm.notify();
    } catch (const std::exception& ex) {
        tberror() << "command line error: " << ex.what() << tbendl();
        return 1;
    }

    // trace a
    if (vm["tracea"].empty()) {
        tberror() << "No trace a specified." << tbendl();
        return 1;
    }
    args.trace_a = vm["tracea"].as<std::vector<std::string>>();

    // trace b
    if (vm["traceb"].empty()) {
        tberror() << "No trace b specified." << tbendl();
        return 1;
    }
    args.trace_b = vm["traceb"].as<std::vector<std::string>>();

    // state providers
    if (!vm["stateprov"].empty()) {
        args.stateProviders = vm["stateprov"].as<std::vector<std::string>>();
    }

    // state providers parameters
    if (!vm["param"].empty()) {
        args.stateProvidersParams = vm["param"].as<std::vector<std::string>>();
    }

    // verbose
    args.verbose = vm["verbose"].as<bool>();

    return 0;
}

}

int main(int argc, char* argv[])
{
    tibee::Arguments args;

    int ret = parseOptions(argc, argv, args);

    if (ret < 0) {
        return 0;
    } else if (ret > 0) {
        return ret;
    }

    // create a TibeeCompare instance and run it.
    try {
        std::unique_ptr<tibee::TibeeCompare> tibeeCompare {new tibee::TibeeCompare {args}};

        return tibeeCompare->run() ? 0 : 1;
    } catch (const tibee::ex::InvalidArgument& ex) {
        tberror() << "invalid argument: " << ex.what() << tbendl();
    } catch (const std::exception& ex) {
        tberror() << "unknown error: " << ex.what() << tbendl();
    }

    return 1;
}
