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
#ifndef _GRAPHBUILDER_NODEPROPERTIES_HPP
#define _GRAPHBUILDER_NODEPROPERTIES_HPP

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <memory>
#include <vector>

#include <common/state/AbstractStateValue.hpp>
#include <common/state/NullStateValue.hpp>
#include <common/state/Uint64StateValue.hpp>
#include <timeline_graph/timeline_node.h>

namespace tibee
{

/**
 * Stores the properties of the nodes of a timeline graph.
 *
 * @author Francois Doray
 */
class NodeProperties
    : public boost::noncopyable
{
public:
    NodeProperties();
    ~NodeProperties();

    const tibee::common::AbstractStateValue& GetProperty(
        timeline_graph::TimelineNodeId node_id,
        const std::string& property_name) const {
      if (node_id >= _properties.size() ||
          _properties[node_id]->find(property_name) ==
              _properties[node_id]->end()) {
        return *_null.get();
      }
      return *(*_properties[node_id])[property_name].get();
    }

    template <typename T>
    void SetProperty(
        timeline_graph::TimelineNodeId node_id,
        const std::string& property_name,
        const T& property_value)
    {
        if (_properties.size() <= node_id)
            _properties.resize(node_id + 1);
        if (!_properties[node_id].get())
            _properties[node_id] = std::unique_ptr<PropertyMap>(
                new PropertyMap());
        (*_properties[node_id])[property_name] =
            tibee::common::AbstractStateValue::UP {new T(property_value)};
    }

    void SetProperty(
        timeline_graph::TimelineNodeId node_id,
        const std::string& property_name,
        const uint64_t& property_value)
    {
        SetProperty(node_id,
                    property_name,
                    tibee::common::Uint64StateValue(property_value));
    }

private:
    // Properties.
    typedef boost::unordered_map<
        std::string, tibee::common::AbstractStateValue::UP> PropertyMap;
    typedef std::vector<std::unique_ptr<PropertyMap>> NodesPropertyMap;
    NodesPropertyMap _properties;

    // Null state value
    tibee::common::NullStateValue::UP _null;
};

}

#endif // _GRAPHBUILDER_NODEPROPERTIES_HPP
