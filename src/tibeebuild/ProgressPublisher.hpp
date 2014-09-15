/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _PROGRESSPUBLISHER_HPP
#define _PROGRESSPUBLISHER_HPP

#include <memory>
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <common/BasicTypes.hpp>
#include <common/trace/TraceSet.hpp>
#include <common/trace/Event.hpp>
#include <common/traceplayback/AbstractTracePlaybackListener.hpp>
#include <common/stateprov/StateProviderConfig.hpp>
#include <common/mq/MqContext.hpp>
#include "StateHistoryBuilder.hpp"
#include "rpc/ProgressUpdateRpcNotification.hpp"
#include "rpc/BuilderJsonRpcMessageEncoder.hpp"

namespace tibee
{

/**
 * Abstract cache builder. All cache builders must inherit this.
 *
 * @author Philippe Proulx
 */
class ProgressPublisher :
    public tibee::common::AbstractTracePlaybackListener
{
public:
    /**
     * Builds a progress publisher.
     *
     * The publisher will check the current system timestamp every
     * \p updatePeriodEvents events. If the difference since the last
     * check is greater than \p updatePeriodMs milliseconds, there will
     * be a progress publication.
     *
     * @param bindAddr            Bind address for publishing progress
     * @param beginTs             Begin timestamp of trace set
     * @param endTs               End timestamp of trace set
     * @param tracesPaths         Paths of all traces
     * @param stateProviders      Descriptors of all state providers
     * @param stateHistoryBuilder State history builder reference
     * @param updatePeriodEvents  Update emission period in number of events
     * @param updatePeriodMs      Update emission period in milliseconds
     */
    ProgressPublisher(const std::string& bindAddr,
                      common::timestamp_t beginTs, common::timestamp_t endTs,
                      const std::vector<boost::filesystem::path>& tracesPaths,
                      const std::vector<common::StateProviderConfig>& stateProviders,
                      const StateHistoryBuilder* stateHistoryBuilder,
                      std::size_t updatePeriodEvents,
                      std::size_t updatePeriodMs);

    ~ProgressPublisher();

protected:
    bool onStartImpl(const common::TraceSet* traceSet);
    void onEventImpl(const common::Event& event);
    bool onStopImpl();
    void publish();

private:
    // bind address
    boost::filesystem::path _bindAddr;

    // number of processed events so far
    std::size_t _evCount;

    // RPC message encoder
    std::unique_ptr<BuilderJsonRpcMessageEncoder> _rpcMessageEncoder;

    // RPC notification (progress update)
    std::unique_ptr<ProgressUpdateRpcNotification> _rpcNotification;

    // state history builder reference
    const StateHistoryBuilder* _stateHistoryBuilder;

    // update period in number of events
    std::size_t _updatePeriodEvents;

    // update period in milliseconds
    std::size_t _updatePeriodMs;

    // temporary event counter
    std::size_t _tmpEvCounter;

    // last event timestamp (set before publishing)
    common::timestamp_t _lastTs;

    // last system time
    boost::posix_time::ptime _lastTime;

    // start time
    boost::posix_time::ptime _startTime;

    // message queue context
    std::unique_ptr<common::MqContext> _mqContext;

    // message queue socket
    std::unique_ptr<common::PublishMqSocket> _mqSocket;
};

}

#endif // _PROGRESSPUBLISHER_HPP
