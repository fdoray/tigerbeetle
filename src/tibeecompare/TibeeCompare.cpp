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
#include <sstream>

#include <common/ex/WrongStateProvider.hpp>
#include <common/trace/CreateTraceSet.hpp>
#include <common/trace/TraceSet.hpp>
#include <common/traceplayback/AbstractTracePlaybackListener.hpp>
#include <common/traceplayback/TracePlayer.hpp>
#include <common/utils/print.hpp>
#include <tibeebuild/StateHistoryBuilder.hpp>
#include <tibeebuild/ex/BuilderBeetleError.hpp>
#include <tibeebuild/ex/StateProviderNotFound.hpp>
#include <tibeebuild/ex/UnknownStateProviderType.hpp>
#include <timeline_graph/timeline_graph.h>
#include "TibeeCompare.hpp"
#include "ex/InvalidArgument.hpp"
#include "graph_builder/GraphBuilder.hpp"
#include "html_writer/HtmlWriter.hpp"

#define THIS_MODULE "compare"

namespace bfs = boost::filesystem;

namespace tibee
{

using common::tbmsg;
using common::tbendl;
using timeline_graph::Matcher;
using timeline_graph::TimelineGraph;

TibeeCompare::TibeeCompare(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void TibeeCompare::validateSaveArguments(const Arguments& args)
{
    // trace paths
    validateSaveTracePaths(args.trace_a, &_traceAPaths);
    validateSaveTracePaths(args.trace_b, &_traceBPaths);

    // verbose
    _verbose = args.verbose;
}

void TibeeCompare::validateSaveTracePaths(const std::vector<std::string>& pathsStr,
                                          std::vector<boost::filesystem::path>* paths)
{
    // make sure all traces actually exist and create paths
    for (const auto& pathStr : pathsStr) {
        auto tracePath = bfs::path {pathStr};

        // make sure this trace exists (at least, may still be invalid)
        if (!bfs::exists(tracePath)) {
            std::stringstream ss;

            ss << "trace " << tracePath << " does not exist";

            throw ex::InvalidArgument {ss.str()};
        }

        // append trace path
        paths->push_back(tracePath);
    }
}

bool TibeeCompare::run()
{
    if (_verbose) {
        tbmsg(THIS_MODULE) << "starting comparison" << tbendl();
    }

    // create a list of trace listeners
    std::vector<common::AbstractTracePlaybackListener::UP> listeners;

    // create a state history builder (if we have at least one provider)
    std::unique_ptr<StateHistoryBuilder> stateHistoryBuilder;
    StateHistoryBuilder* shbPtr = nullptr;

    std::vector<common::StateProviderConfig> stateProviders;
    stateProviders.push_back(common::StateProviderConfig{"src/providers/linux/linux.so", "linux"});

    if (!stateProviders.empty()) {
        try {
            stateHistoryBuilder = std::unique_ptr<StateHistoryBuilder> {
                new StateHistoryBuilder {
                    "",
                    stateProviders
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

    // create a graph builder
    std::unique_ptr<GraphBuilder> graphBuilder(new GraphBuilder());
    GraphBuilder* graphBuilderPtr = graphBuilder.get();
    listeners.push_back(std::move(graphBuilder));
    shbPtr->SetStateChangeSink(graphBuilderPtr);

    // build graphs for the 2 traces
    common::TracePlayer player;

    tibee::common::TraceSet::UP traceSetA =
        tibee::common::CreateTraceSet(_traceAPaths);
    player.play(traceSetA.get(), listeners);
    auto graphA = graphBuilderPtr->TakeGraph();
    auto propertiesA = graphBuilderPtr->TakeProperties();

    tibee::common::TraceSet::UP traceSetB =
        tibee::common::CreateTraceSet(_traceBPaths);
    player.play(traceSetB.get(), listeners);
    auto graphB = graphBuilderPtr->TakeGraph();
    auto propertiesB = graphBuilderPtr->TakeProperties();

    // match the graphs
    Matcher matcher;
    matcher.MatchGraphs(*graphA.get(), *graphB.get());

    // write the comparison result
    HtmlWriter writer;
    writer.WriteHtml("comparison.html",
                     *graphA,
                     *graphB,
                     *propertiesA,
                     *propertiesB,
                     matcher);

    return true;
}

}
