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
#ifndef _TIBEE_MQ_PUBLISHMQSOCKET_HPP
#define _TIBEE_MQ_PUBLISHMQSOCKET_HPP

#include <zmq.h>

#include "mq/AbstractMqSocket.hpp"

namespace tibee
{
namespace mq
{

class MqContext;

/**
 * Publish message queue socket.
 *
 * A publish socket is used by a publisher to distribute data. Messages
 * sent are distributed in a fan out fashion to all connected peers.
 * The recv() function is not implemented for this socket type.
 *
 * @author Philippe Proulx
 */
class PublishMqSocket :
    public AbstractMqSocket
{
    friend class MqContext;

private:
    /**
     * Builds a publish socket.
     */
    PublishMqSocket(MqContext* context) :
        AbstractMqSocket {context, ZMQ_PUB}
    {
    }
};

}
}

#endif // _TIBEE_MQ_PUBLISHMQSOCKET_HPP
