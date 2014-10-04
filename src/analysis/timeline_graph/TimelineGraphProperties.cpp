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
#include "analysis/timeline_graph/TimelineGraphProperties.hpp"

#include "base/print.hpp"
#include "value/MakeValue.hpp"

namespace tibee {
namespace analysis {
namespace timeline_graph {

TimelineGraphProperties::TimelineGraphProperties()
{
}

TimelineGraphProperties::~TimelineGraphProperties() {
}

const value::Value* TimelineGraphProperties::GetProperty(
    NodeId node_id,
    quark::Quark property_name) const
{
    if (node_id >= _properties.size() ||
        _properties[node_id].get() == nullptr) {
        return nullptr;
    }

    auto look = _properties[node_id]->find(property_name);
    if (look == _properties[node_id]->end())
        return nullptr;

    return look->second.get();
}

value::Value* TimelineGraphProperties::GetProperty(
    NodeId node_id,
    quark::Quark property_name)
{
    if (node_id >= _properties.size() ||
        _properties[node_id].get() == nullptr) {
        return nullptr;
    }

    auto look = _properties[node_id]->find(property_name);
    if (look == _properties[node_id]->end())
        return nullptr;

    return look->second.get();
}

const TimelineGraphProperties::PropertyMap& TimelineGraphProperties::GetProperties(
        NodeId node_id) const
{
    assert(node_id < _properties.size());
    return *_properties[node_id];
}

void TimelineGraphProperties::SetProperty(
    NodeId node_id,
    quark::Quark property_name,
    value::Value::UP property_value)
{
    if (_properties.size() <= node_id)
        _properties.resize(node_id + 1);

    if (!_properties[node_id].get()) {
        _properties[node_id] = std::unique_ptr<PropertyMap>(
            new PropertyMap());
    }

    (*_properties[node_id])[property_name] = std::move(property_value); 
}

void TimelineGraphProperties::IncrementProperty(
    NodeId node_id,
    quark::Quark property_name,
    uint64_t increment)
{

    auto value = GetProperty(node_id, property_name);

    if (value == nullptr)
    {
        SetProperty(node_id, property_name, value::MakeValue(increment));
    }
    else if (value::ULongValue::InstanceOf(value))
    {
        auto ulong_value = value::ULongValue::Cast(value);
        uint64_t new_value = ulong_value->GetValue() + increment;
        ulong_value->SetValue(new_value);
    }
    else
    {
        base::tberror() << "Cannot increment a value that is not a ULongValue." << base::tbendl();
    }
}

}  // namespace timeline_graph
}  // namespace analysis
}  // namespace tibee
