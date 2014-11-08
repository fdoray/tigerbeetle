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
#ifndef _TIBEE_METRICBLOCKS_ABSTRACTMETRICBLOCK_HPP
#define _TIBEE_METRICBLOCKS_ABSTRACTMETRICBLOCK_HPP

#include "block/AbstractBlock.hpp"
#include "execution/ExecutionBuilder.hpp"
#include "state/CurrentState.hpp"

namespace tibee {
namespace metric_blocks {

/**
 * Abstract metric block.
 *
 * @author Francois Doray
 */
class AbstractMetricBlock : public block::AbstractBlock
{
public:
    AbstractMetricBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;

protected:
    // Current state.
    state::CurrentState* State() const { return _currentState; }

    // Execution builder.
    execution::ExecutionBuilder* Builder() const { return _executionBuilder; }

private:
    // Current state.
    state::CurrentState* _currentState;

    // Graph builder.
    execution::ExecutionBuilder* _executionBuilder;
};

}  // namespace metric_blocks
}  // namespace tibee

#endif // _TIBEE_METRICBLOCKS_ABSTRACTMETRICBLOCK_HPP
