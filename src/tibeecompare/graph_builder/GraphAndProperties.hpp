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

#ifndef _GRAPHBUILDER_GRAPHANDPROPERTIES_HPP
#define _GRAPHBUILDER_GRAPHANDPROPERTIES_HPP

#include <memory>

#include <tibeecompare/graph_builder/GraphProperties.hpp>
#include <timeline_graph/timeline_graph.h>

namespace tibee {

struct GraphAndProperties {

    typedef std::unique_ptr<GraphAndProperties> UP;

    // The nodes and edges of the graph.
    timeline_graph::TimelineGraph graph;

    // The properties of the nodes of the graph.
    GraphProperties properties;

};

}  // namespace tibee

#endif  // _GRAPHBUILDER_GRAPHANDPROPERTIES_HPP
