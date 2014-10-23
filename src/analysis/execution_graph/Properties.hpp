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
#ifndef _TIBEE_ANALYSIS_EXECUTIONGRAPH_PROPERTIES_HPP
#define _TIBEE_ANALYSIS_EXECUTIONGRAPH_PROPERTIES_HPP

#include <boost/noncopyable.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "analysis/execution_graph/Node.hpp"
#include "analysis/execution_graph/NodeStepKey.hpp"
#include "quark/Quark.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

/**
 * Stores the properties of the nodes of an execution graph.
 *
 * @author Francois Doray
 */
class Properties
    : public boost::noncopyable
{
public:
    typedef std::unordered_map<quark::Quark, value::Value::UP> PropertyMap;

    Properties();
    ~Properties();

    const value::Value* GetProperty(
        const NodeStepKey& node_step_key,
        quark::Quark property_name) const;

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

private:
    value::Value* GetProperty(
        const NodeStepKey& node_step_key,
        quark::Quark property_name);

    bool HasPropertyMap(const NodeStepKey& node_step_key) const;

    // Properties.
    typedef std::vector<std::unique_ptr<PropertyMap>> StepsPropertyMap;
    typedef std::vector<StepsPropertyMap> NodesPropertyMap;
    NodesPropertyMap _properties;
};

}  // namespace execution_graph
}  // namespace analysis
}  // namespace tibee

#endif // _TIBEE_ANALYSIS_EXECUTIONGRAPH_PROPERTIES_HPP
