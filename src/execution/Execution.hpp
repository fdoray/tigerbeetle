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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.    If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_EXECUTION_EXECUTION_HPP
#define _TIBEE_EXECUTION_EXECUTION_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "execution/Graph.hpp"
#include "execution/NodeProperties.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace execution {

class Execution
{
public:
    typedef std::unique_ptr<Execution> UP;

    Execution(const std::string& description);

    const std::string& description() const { return _description; }

    Graph& graph() { return _graph; }
    const Graph& graph() const { return _graph; }

    NodeProperties& node_properties() { return _node_properties; }
    const NodeProperties& node_properties() const { return _node_properties; }

    const value::Value* GetMetric(const std::string& name) const;
    void SetMetric(const std::string& name, value::Value::UP value);

private:
    std::string _description;
    Graph _graph;
    NodeProperties _node_properties;
    std::unordered_map<std::string, value::Value::UP> _metrics;
};

typedef std::vector<Execution::UP> Executions;

}  // namespace execution
}  // namespace tibee

#endif  // _TIBEE_EXECUTION_EXECUTION_HPP
