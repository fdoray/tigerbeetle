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
#include "execution/Execution.hpp"

namespace tibee {
namespace execution {

Execution::Execution(const std::string& description)
        : _description(description) {}

const value::Value* Execution::GetMetric(quark::Quark name) const
{
    auto look = _metrics.find(name);
    if (look == _metrics.end())
        return nullptr;
    return look->second.get();
}

value::Value* Execution::GetMetric(quark::Quark name)
{
    auto look = _metrics.find(name);
    if (look == _metrics.end())
        return nullptr;
    return look->second.get();
}

void Execution::SetMetric(quark::Quark name, value::Value::UP value)
{
    _metrics[name] = std::move(value);
}

}  // namespace execution
}  // namespace tibee
