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
#include <tibeecompare/json_writer/JsonWriter.hpp>

#include <boost/format.hpp>
#include <fstream>
#include <json/json.h>
#include <string>

namespace tibee
{

using boost::format;

namespace
{

void WriteGraph(const timeline_graph::TimelineGraph& graph,
                const GraphProperties& graph_properties,
                uint64_t* tid,
                Json::Value* events) {
    assert(tid);
    assert(events);

    uint64_t pid = *tid;

    auto it = graph.depth_first_search_begin();
    auto end = graph.depth_first_search_end();

    std::vector<uint64_t> tid_for_depth;
    std::vector<uint64_t> ts_for_depth;

    for (; it != end; ++it) {
        // Generate a new pid if required.
        size_t depth = it.Depth();
        if (tid_for_depth.size() < depth + 1) {
            tid_for_depth.push_back(*tid);
            ++(*tid);
        }

        // Get the timestamp from the parent if required.
        if (ts_for_depth.size() < depth + 1) {
            if (ts_for_depth.empty())
                ts_for_depth.push_back(0);
            else
                ts_for_depth.push_back(ts_for_depth.back());
        }

        // Read properties.
        auto& duration_wrapper = graph_properties.GetProperty(it->id(), "duration");
        uint64_t duration = 0;
        if (!duration_wrapper.isNull())
            duration = duration_wrapper.asUint64();

        // Write the event.
        Json::Value& event = events->append(Json::Value());
        event["name"] = Json::Value(std::string("Node ") + std::to_string(it->id()));
        event["cat"] = Json::Value("comparison");
        event["ph"] = Json::Value("X");
        event["pid"] = Json::Value(static_cast<Json::UInt64>(pid));
        event["tid"] = Json::Value(static_cast<Json::UInt64>(tid_for_depth[depth]));
        event["ts"] = Json::Value(static_cast<Json::UInt64>(ts_for_depth[depth]));
        event["dur"] = Json::Value(static_cast<Json::UInt64>(duration));

        Json::Value& args = event["args"];
        args["hello"] = Json::Value("world");

        // Compute the timestamp for the next event of this depth.
        ts_for_depth[depth] += duration;

        // Discard pids and ts that cannot be reused.
        tid_for_depth.resize(depth + 1);
        ts_for_depth.resize(depth + 1);
    }
}

}  // namespace

JsonWriter::JsonWriter() {
}

JsonWriter::~JsonWriter() {
}

void JsonWriter::WriteJson(const boost::filesystem::path& out_file,
                           const timeline_graph::TimelineGraph& graph_a,
                           const timeline_graph::TimelineGraph& graph_b,
                           const GraphProperties& graph_a_properties,
                           const GraphProperties& graph_b_properties,
                           const timeline_graph::Matcher& matcher)
{
    Json::StyledStreamWriter writer;

    // Create the Json tree.
    Json::Value root;
    Json::Value& events = root["traceEvents"];
    uint64_t tid = 1;

    // Generate events for both graphs.
    WriteGraph(graph_a, graph_a_properties, &tid, &events);
    WriteGraph(graph_b, graph_b_properties, &tid, &events);

    // Write the Json file.
    std::ofstream out_a("comparison.json", std::ofstream::out);
    writer.write(out_a, root);
    out_a.close();
}

}
