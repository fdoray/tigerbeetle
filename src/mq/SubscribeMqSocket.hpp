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
#ifndef _TIBEE_MQ_SUBSCRIBEMQSOCKET_HPP
#define _TIBEE_MQ_SUBSCRIBEMQSOCKET_HPP

#include <zmq.h>

#include "mq/AbstractMqSocket.hpp"

namespace tibee
{
namespace mq
{

class MqContext;

/**
 * Subscribe message queue socket.
 *
 * A subscribe socket is used by a subscriber to subscribe to data
 * distributed by a publisher. The send() function is not implemented
 * for this socket type.
 *
 * @author Philippe Proulx
 */
class SubscribeMqSocket :
    public AbstractMqSocket
{
    friend class MqContext;

private:
    /**
     * Builds a subscribe socket.
     */
    SubscribeMqSocket(MqContext* context) :
        AbstractMqSocket {context, ZMQ_SUB}
    {
        if (this->getInternalSocket()) {
            // subscribe to all incoming messages
            ::zmq_setsockopt(this->getInternalSocket(), ZMQ_SUBSCRIBE, nullptr, 0);
        }
    }
};

}
}

#endif // _TIBEE_MQ_SUBSCRIBEMQSOCKET_HPP
