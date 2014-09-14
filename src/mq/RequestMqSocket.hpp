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
#ifndef _TIBEE_MQ_REQUESTMQSOCKET_HPP
#define _TIBEE_MQ_REQUESTMQSOCKET_HPP

#include <zmq.h>

#include "mq/AbstractMqSocket.hpp"

namespace tibee
{
namespace mq
{

class MqContext;

/**
 * Request message queue socket.
 *
 * A request socket is used by a client to send requests to and receive
 * replies from a service. This socket type allows only an alternating
 * sequence of send(request) and subsequent recv(reply) calls.
 *
 * @author Philippe Proulx
 */
class RequestMqSocket :
    public AbstractMqSocket
{
    friend class MqContext;

private:
    /**
     * Builds a request socket.
     */
    RequestMqSocket(MqContext* context) :
        AbstractMqSocket {context, ZMQ_REQ}
    {
    }
};

}
}

#endif // _TIBEE_MQ_REQUESTMQSOCKET_HPP
