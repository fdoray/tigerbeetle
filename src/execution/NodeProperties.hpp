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
#ifndef _TIBEE_EXECUTION_NODEPROPERTIES_HPP
#define _TIBEE_EXECUTION_NODEPROPERTIES_HPP

#include <boost/noncopyable.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "execution/Node.hpp"
#include "execution/NodeStepKey.hpp"
#include "quark/Quark.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace execution {

/**
 * Stores the properties of the nodes of an execution graph.
 *
 * @author Francois Doray
 */
class NodeProperties
    : public boost::noncopyable
{
public:
    typedef std::unordered_map<quark::Quark, value::Value::UP> PropertyMap;

    NodeProperties();
    ~NodeProperties();

    const value::Value* GetProperty(
        const NodeStepKey& node_step_key,
        quark::Quark property_name) const;

    value::Value* GetProperty(
        const NodeStepKey& node_step_key,
        quark::Quark property_name);

    const PropertyMap& GetProperties(
        const NodeStepKey& node_step_key) const;

    void SetProperty(
        const NodeStepKey& node_step_key,
        quark::Quark property_name,
        value::Value::UP property_value);

    void IncrementProperty(
        const NodeStepKey& node_step_key,
        quark::Quark property_name,
        uint64_t increment);

    bool HasPropertyMap(const NodeStepKey& node_step_key) const;

    size_t size() const { return _properties.size(); }

private:

    // Properties.
    typedef std::vector<std::unique_ptr<PropertyMap>> StepsPropertyMap;
    typedef std::vector<StepsPropertyMap> NodesPropertyMap;
    NodesPropertyMap _properties;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_NODEPROPERTIES_HPP
