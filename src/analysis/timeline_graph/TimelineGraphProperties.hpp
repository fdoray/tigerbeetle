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
#ifndef _TIBEE_ANALYSIS_TIMELINEGRAPH_TIMELINEGRAPHPROPERTIES_HPP
#define _TIBEE_ANALYSIS_TIMELINEGRAPH_TIMELINEGRAPHPROPERTIES_HPP

#include <boost/noncopyable.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "analysis/timeline_graph/Node.hpp"
#include "quark/Quark.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

/**
 * Stores the properties of the nodes of a timeline graph.
 *
 * @author Francois Doray
 */
class TimelineGraphProperties
    : public boost::noncopyable
{
public:
    typedef std::unordered_map<quark::Quark, value::Value::UP> PropertyMap;

    TimelineGraphProperties();
    ~TimelineGraphProperties();

    const value::Value* GetProperty(
        NodeId node_id,
        quark::Quark property_name) const;

    value::Value* GetProperty(
        NodeId node_id,
        quark::Quark property_name);

    const PropertyMap& GetProperties(
        NodeId node_id) const;

    void SetProperty(
        NodeId node_id,
        quark::Quark property_name,
        value::Value::UP property_value);

    void IncrementProperty(
        NodeId node_id,
        quark::Quark property_name,
        uint64_t increment);

private:
    // Properties.
    typedef std::vector<std::unique_ptr<PropertyMap>> NodesPropertyMap;
    NodesPropertyMap _properties;
};

}  // namespace timeline_graph
}  // namespace analysis
}  // namespace tibee

#endif // _TIBEE_ANALYSIS_TIMELINEGRAPH_TIMELINEGRAPHPROPERTIES_HPP
