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
#ifndef _TIBEE_METRICBLOCKS_LINUXSTATUSMETRICBLOCK_HPP
#define _TIBEE_METRICBLOCKS_LINUXSTATUSMETRICBLOCK_HPP

#include "base/BasicTypes.hpp"
#include "block/AbstractBlock.hpp"
#include "execution/ExecutionBuilder.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"
#include "state/AttributeKey.hpp"
#include "state/CurrentState.hpp"

namespace tibee {
namespace metric_blocks {

/**
 * Linux status metric block.
 *
 * @author Francois Doray
 */
class LinuxStatusMetricBlock : public block::AbstractBlock
{
public:
    LinuxStatusMetricBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onStatusChange(const notification::Path& path, const value::Value* value);

    // Current state.
    state::CurrentState* _currentState;

    // Graph builder.
    execution::ExecutionBuilder* _executionBuilder;

    // Constant quarks.
    quark::Quark Q_LINUX;
    quark::Quark Q_THREADS;
    quark::Quark Q_STATUS;
};

}  // namespace metric_blocks
}  // namespace tibee

#endif // _TIBEE_METRICBLOCKS_LINUXSTATUSMETRICBLOCK_HPP
