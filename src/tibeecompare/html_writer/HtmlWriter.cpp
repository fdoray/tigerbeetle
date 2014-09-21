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
#include <tibeecompare/html_writer/HtmlWriter.hpp>

#include <boost/format.hpp>
#include <fstream>
#include <string>



#include <iostream>


namespace tibee
{

using boost::format;

namespace
{
const char kHtmlHeader[] =
    "<!doctype html>\n"
    "<html>\n"
    "<head>\n"
    "  <title>Trace comparison</title>\n"
    "</head>\n"
    "<body>\n"
    "  <table>\n"
    "    <tr>\n"
    "      <th>Node</th>\n"
    "      <th>Time A</th>\n"
    "      <th>Time B</th>\n"
    "      <th>Time diff</th>\n"
    "      <th>User A</th>\n"
    "      <th>User B</th>\n"
    "      <th>User diff</th>\n"
    "      <th>Syscall A</th>\n"
    "      <th>Syscall B</th>\n"
    "      <th>Syscall diff</th>\n"
    "      <th>Interrupted A</th>\n"
    "      <th>Interrupted B</th>\n"
    "      <th>Interrupted diff</th>\n"
    "      <th>Blocked A</th>\n"
    "      <th>Blocked B</th>\n"
    "      <th>Blocked diff</th>\n"
    "      <th>Wait CPU A</th>\n"
    "      <th>Wait CPU B</th>\n"
    "      <th>Wait diff</th>\n"
    "    </tr>\n";

const char kHtmlNode[] =
    "    <tr>\n"
    "      <td>%s %d</td>\n"
    "      <td>%d</td>\n"
    "      <td>%d</td><td>%s</td>\n"
    "      <td>%d</td>\n"
    "      <td>%d</td><td>%s</td>\n"
    "      <td>%d</td>\n"
    "      <td>%d</td><td>%s</td>\n"
    "      <td>%d</td>\n"
    "      <td>%d</td><td>%s</td>\n"
    "      <td>%d</td>\n"
    "      <td>%d</td><td>%s</td>\n"
    "      <td>%d</td>\n"
    "      <td>%d</td><td>%s</td>\n"
    "    </tr>\n";

const char kHtmlFooter[] =
    "  </table>\n"
    "</body>\n"
    "</html>\n";

// TODO: Temporary hack.
uint64_t GetUint64(const common::AbstractStateValue& val) {
  if (val)
    return val.asUint64();
  return 0;
}

std::string GetComparison(uint64_t a, uint64_t b) {
    if (a < b) {
        return (format("<span style='color:red; font-size:0.8em;'>+%d</span>") % (b-a)).str();
    } else if (a > b) {
        return (format("<span style='color:green; font-size:0.8em;'>-%d</span>") % (a-b)).str();
    }
    return "<span style='color:grey; font-size:0.8em;'>=</span>";
}

}

HtmlWriter::HtmlWriter() {
}

HtmlWriter::~HtmlWriter() {
}

void HtmlWriter::WriteHtml(const boost::filesystem::path& out_file,
                           const timeline_graph::TimelineGraph& graph_a,
                           const timeline_graph::TimelineGraph& graph_b,
                           const GraphProperties& graph_a_properties,
                           const GraphProperties& graph_b_properties,
                           const timeline_graph::Matcher& matcher)
{
  // Open the output file.
  std::ofstream out(out_file.c_str(), std::ofstream::out);

  // Write the file header.
  out << kHtmlHeader;

  // Write the nodes from trace A.
  for (auto it = graph_a.depth_first_search_begin();
       it != graph_a.depth_first_search_end();
       ++it) {
    // Get duration of nodes in both graphs.
    uint64_t graph_a_duration = GetUint64(graph_a_properties.GetProperty(
        it->id(), "duration"));
    uint64_t graph_b_duration = GetUint64(graph_b_properties.GetProperty(
        matcher.GetGraphBForGraphA(it->id()), "duration"));

    uint64_t graph_a_user = GetUint64(graph_a_properties.GetProperty(
        it->id(), "usermode"));
    uint64_t graph_b_user = GetUint64(graph_b_properties.GetProperty(
        matcher.GetGraphBForGraphA(it->id()), "usermode"));

    uint64_t graph_a_syscall = GetUint64(graph_a_properties.GetProperty(
        it->id(), "syscall"));
    uint64_t graph_b_syscall = GetUint64(graph_b_properties.GetProperty(
        matcher.GetGraphBForGraphA(it->id()), "syscall"));

    uint64_t graph_a_interrupted = GetUint64(graph_a_properties.GetProperty(
        it->id(), "interrupted"));
    uint64_t graph_b_interrupted = GetUint64(graph_b_properties.GetProperty(
        matcher.GetGraphBForGraphA(it->id()), "interrupted"));

    uint64_t graph_a_blocked = GetUint64(graph_a_properties.GetProperty(
        it->id(), "wait-blocked"));
    uint64_t graph_b_blocked = GetUint64(graph_b_properties.GetProperty(
        matcher.GetGraphBForGraphA(it->id()), "wait-blocked"));

    uint64_t graph_a_wait_cpu = GetUint64(graph_a_properties.GetProperty(
        it->id(), "wait-for-cpu"));
    uint64_t graph_b_wait_cpu = GetUint64(graph_b_properties.GetProperty(
        matcher.GetGraphBForGraphA(it->id()), "wait-for-cpu"));

    // Generate indentation string.
    std::string indentation;
    for (size_t i = 0; i < it.Depth(); ++i)
      indentation += "&nbsp;&nbsp;";

    // Write HTML.
    out << format(kHtmlNode) %
        (indentation + "Node") %
        it->id() %
        graph_a_duration %
        graph_b_duration %
        GetComparison(graph_a_duration, graph_b_duration) %
        graph_a_user %
        graph_b_user %
        GetComparison(graph_a_user, graph_b_user) %
        graph_a_syscall %
        graph_b_syscall %
        GetComparison(graph_a_syscall, graph_b_syscall) %
        graph_a_interrupted %
        graph_b_interrupted %
        GetComparison(graph_a_interrupted, graph_b_interrupted) %
        graph_a_blocked %
        graph_b_blocked %
        GetComparison(graph_a_blocked, graph_b_blocked) %
        graph_a_wait_cpu %
        graph_b_wait_cpu %
        GetComparison(graph_a_wait_cpu, graph_b_wait_cpu);
  }

  // Write the file footer.
  out << kHtmlFooter;

  // Close the output file.
  out.close();
}

}
