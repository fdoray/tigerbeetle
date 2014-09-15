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
#ifndef _HTMLWRITER_HTMLWRITER_HPP
#define _HTMLWRITER_HTMLWRITER_HPP

#include <boost/filesystem/path.hpp>

#include <tibeecompare/graph_builder/NodeProperties.hpp>
#include <timeline_graph/timeline_graph.h>
#include <timeline_graph/matcher.h>

namespace tibee
{

class HtmlWriter {
public:
    HtmlWriter();
    ~HtmlWriter();

    void WriteHtml(const boost::filesystem::path& out_file,
                   const timeline_graph::TimelineGraph& graph_a,
                   const timeline_graph::TimelineGraph& graph_b,
                   const NodeProperties& graph_a_properties,
                   const NodeProperties& graph_b_properties,
                   const timeline_graph::Matcher& matcher);
};

}

#endif // _ARGUMENTS_HPP
