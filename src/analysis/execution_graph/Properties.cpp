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
#include "analysis/execution_graph/Properties.hpp"

#include "base/print.hpp"
#include "value/MakeValue.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

Properties::Properties()
{
}

Properties::~Properties() {
}

const value::Value* Properties::GetProperty(
    const NodeStepKey& node_step_key,
    quark::Quark property_name) const
{
    if (!HasPropertyMap(node_step_key))
        return nullptr;

    const PropertyMap& property_map =
        *_properties[node_step_key.node_id()][node_step_key.step_index()];

    auto look = property_map.find(property_name);
    if (look == property_map.end())
        return nullptr;

    return look->second.get();
}

value::Value* Properties::GetProperty(
    const NodeStepKey& node_step_key,
    quark::Quark property_name)
{
    if (!HasPropertyMap(node_step_key))
        return nullptr;

    PropertyMap& property_map =
        *_properties[node_step_key.node_id()][node_step_key.step_index()];

    auto look = property_map.find(property_name);
    if (look == property_map.end())
        return nullptr;

    return look->second.get();
}

const Properties::PropertyMap& Properties::GetProperties(
    const NodeStepKey& node_step_key) const
{
    assert(HasPropertyMap(node_step_key));
    return *_properties[node_step_key.node_id()][node_step_key.step_index()];
}

void Properties::SetProperty(
    const NodeStepKey& node_step_key,
    quark::Quark property_name,
    value::Value::UP property_value)
{
    // Allocate the property map for the |node_step_key|.
    if (_properties.size() <= node_step_key.node_id())
        _properties.resize(node_step_key.node_id() + 1);
    if (_properties[node_step_key.node_id()].size() <= node_step_key.step_index())
        _properties[node_step_key.node_id()].resize(node_step_key.step_index() + 1);
    if (_properties[node_step_key.node_id()][node_step_key.step_index()].get() == nullptr)
        _properties[node_step_key.node_id()][node_step_key.step_index()].reset(new PropertyMap());

    // Set the property.
    (*_properties[node_step_key.node_id()][node_step_key.step_index()])[property_name] =
        std::move(property_value); 
}

void Properties::IncrementProperty(
    const NodeStepKey& node_step_key,
    quark::Quark property_name,
    uint64_t increment)
{
    auto value = GetProperty(node_step_key, property_name);

    if (value == nullptr)
    {
        SetProperty(node_step_key, property_name, value::MakeValue(increment));
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

bool Properties::HasPropertyMap(const NodeStepKey& node_step_key) const
{
    if (node_step_key.node_id() >= _properties.size() ||
        node_step_key.step_index() >= _properties[node_step_key.node_id()].size() ||
        _properties[node_step_key.node_id()][node_step_key.step_index()].get() == nullptr)
    {
        return false;
    }   
    return true;
}

}  // namespace execution_graph
}  // namespace analysis
}  // namespace tibee
