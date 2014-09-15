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
    "    </tr>\n";

const char kHtmlNode[] =
    "    <tr>\n"
    "      <td>%s %d</td>\n"
    "      <td>%d</td>\n"
    "      <td>%d</td>\n"
    "    </tr>\n";

const char kHtmlFooter[] =
    "  </table>\n"
    "</body>\n"
    "</html>\n";
}

HtmlWriter::HtmlWriter() {
}

HtmlWriter::~HtmlWriter() {
}

void HtmlWriter::WriteHtml(const boost::filesystem::path& out_file,
                           const timeline_graph::TimelineGraph& graph_a,
                           const timeline_graph::TimelineGraph& graph_b,
                           const NodeProperties& graph_a_properties,
                           const NodeProperties& graph_b_properties,
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
    uint64_t graph_a_duration = graph_a_properties.GetProperty(
        it->id(), "duration").asUint64();
    uint64_t graph_b_duration = graph_b_properties.GetProperty(
        matcher.GetGraphBForGraphA(it->id()), "duration").asUint64();

    // Generate indentation string.
    std::string indentation;
    for (size_t i = 0; i < it.Depth(); ++i)
      indentation += "&nbsp;&nbsp;";

    // Write HTML.
    out << format(kHtmlNode) %
        (indentation + "Node") %
        it->id() %
        graph_a_duration %
        graph_b_duration;
  }

  // Write the file footer.
  out << kHtmlFooter;

  // Close the output file.
  out.close();
}

}
