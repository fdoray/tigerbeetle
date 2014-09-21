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
#include "GraphProperties.hpp"

namespace tibee
{

GraphProperties::GraphProperties()
    : _null(tibee::common::NullStateValue::UP(
        new tibee::common::NullStateValue())) {
}

GraphProperties::~GraphProperties() {
}

const tibee::common::AbstractStateValue& GraphProperties::GetProperty(
    timeline_graph::TimelineNodeId node_id,
    const std::string& property_name) const {

    if (node_id >= _properties.size() ||
        _properties[node_id].get() == nullptr) {
        return *_null;
    }

    auto look = _properties[node_id]->find(property_name);
    if (look == _properties[node_id]->end())
        return *_null;

    return *look->second;
}

void GraphProperties::SetProperty(
    timeline_graph::TimelineNodeId node_id,
    const std::string& property_name,
    const uint64_t& property_value) {
    SetProperty(node_id,
                property_name,
                tibee::common::Uint64StateValue(property_value));
}

void GraphProperties::IncrementProperty(
    timeline_graph::TimelineNodeId node_id,
    const std::string& property_name,
    uint64_t increment) {

    auto& currentValueWrapper = GetProperty(node_id, property_name);

    uint64_t currentValue = 0;
    if (!currentValueWrapper.isNull()) {
        assert(currentValueWrapper.isUint64());
        currentValue = currentValueWrapper.asUint64();
    }

    SetProperty(node_id, property_name, currentValue + increment);
}

const GraphProperties::PropertyMap& GraphProperties::GetNodeProperties(
        timeline_graph::TimelineNodeId node_id) const {
    assert(node_id < _properties.size());
    return *_properties[node_id];
}

}  // namespace tibee
