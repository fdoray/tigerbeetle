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
#ifndef _TIBEE_ANALYSISBLOCKS_GRAPHBUILDERBLOCK_HPP
#define _TIBEE_ANALYSISBLOCKS_GRAPHBUILDERBLOCK_HPP

#include "analysis/execution_graph/GraphBuilder.hpp"
#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"

namespace tibee {
namespace analysis_blocks {

class GraphBuilderBlock : public block::AbstractBlock
{
public:
    GraphBuilderBlock();
    ~GraphBuilderBlock();

private:
    virtual void RegisterServices(block::ServiceList* serviceList) override;
    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;
    virtual void GetNotificationSinks(notification::NotificationCenter* notificationCenter) override;

    void onTimestamp(const notification::Path& path, const value::Value* value);
    void onEnd(const notification::Path& path, const value::Value* value);

    // The graphs that are being built.
    analysis::execution_graph::GraphBuilder _graphBuilder;

    // Sink to send completed graphs.
    const notification::NotificationSink* _graphSink;
};

}  // namespace analysis_blocks
}  // namespace tibee

#endif // _TIBEE_ANALYSISBLOCKS_GRAPHBUILDERBLOCK_HPP
