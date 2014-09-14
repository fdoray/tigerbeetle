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
#ifndef _TIBEE_MQ_MQCONTEXT_HPP
#define _TIBEE_MQ_MQCONTEXT_HPP

#include <boost/utility.hpp>
#include <memory>

#include "mq/RequestMqSocket.hpp"
#include "mq/ReplyMqSocket.hpp"
#include "mq/PublishMqSocket.hpp"
#include "mq/SubscribeMqSocket.hpp"

namespace tibee
{
namespace mq
{

/**
 * Message queue context.
 *
 * Use a message queue context to create message queue sockets.
 *
 * @author Philippe Proulx
 */
class MqContext :
    boost::noncopyable
{
    friend class AbstractMqSocket;
public:
    /**
     * Builds a message queue context.
     *
     * @param ioThreads Number of I/O threads to spawn (at least 1)
     */
    MqContext(std::size_t ioThreads);

    ~MqContext();

    /**
     * Creates and returns a request socket.
     *
     * @returns New request socket or \a nullptr if any error occured
     */
    std::unique_ptr<RequestMqSocket> createRequestSocket();

    /**
     * Creates and returns a reply socket.
     *
     * @returns New reply socket or \a nullptr if any error occured
     */
    std::unique_ptr<ReplyMqSocket> createReplySocket();

    /**
     * Creates and returns a publish socket.
     *
     * @returns New publish socket or \a nullptr if any error occured
     */
    std::unique_ptr<PublishMqSocket> createPublishSocket();

    /**
     * Creates and returns a subscribe socket.
     *
     * @returns New subscribe socket or \a nullptr if any error occured
     */
    std::unique_ptr<SubscribeMqSocket> createSubscribeSocket();

private:
    // internal context
    void* _context;
};

}
}

#endif // _TIBEE_MQ_MQCONTEXT_HPP
