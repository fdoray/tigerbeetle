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
#ifndef _TIBEE_MQ_REPLYMQSOCKET_HPP
#define _TIBEE_MQ_REPLYMQSOCKET_HPP

#include <zmq.h>

#include "mq/AbstractMqSocket.hpp"

namespace tibee
{
namespace mq
{

class MqContext;

/**
 * Reply message queue socket.
 *
 * A reply socket is used by a service to receive requests from and send
 * replies to a client. This socket type allows only an alternating
 * sequence of recv(request) and subsequent send(reply) calls.
 *
 * @author Philippe Proulx
 */
class ReplyMqSocket :
    public AbstractMqSocket
{
    friend class MqContext;

private:
    /**
     * Builds a reply socket.
     */
    ReplyMqSocket(MqContext* context) :
        AbstractMqSocket {context, ZMQ_REP}
    {
    }
};

}
}

#endif // _TIBEE_MQ_REPLYMQSOCKET_HPP
