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
#include <cstddef>
#include <zmq.h>

#include "mq/ex/MqSocket.hpp"
#include "mq/MqContext.hpp"
#include "mq/AbstractMqSocket.hpp"

namespace tibee
{
namespace mq
{

AbstractMqSocket::AbstractMqSocket(MqContext* context, int socketType)
{
    _socket = ::zmq_socket(context->_context, socketType);

    if (!_socket) {
        throw ex::MqSocket {"cannot create message queue socket"};
    }
}

AbstractMqSocket::~AbstractMqSocket()
{
    if (_socket) {
        this->close();
    }
}

bool AbstractMqSocket::connect(const std::string& addr)
{
    auto ret = ::zmq_connect(_socket, addr.c_str());

    return ret == 0;
}

bool AbstractMqSocket::bind(const std::string& addr)
{
    auto ret = ::zmq_bind(_socket, addr.c_str());

    return ret == 0;
}

bool AbstractMqSocket::close()
{
    auto ret = ::zmq_close(_socket);
    _socket = nullptr;

    return ret == 0;
}

MqMessage::UP AbstractMqSocket::recv()
{
    MqMessage::UP msg {new MqMessage};

    auto ret = ::zmq_recvmsg(_socket, msg->getInternalMessage(), 0);

    if (ret < 0) {
        return nullptr;
    }

    return msg;
}

bool AbstractMqSocket::send(MqMessage::UP msg)
{
    auto ret = ::zmq_sendmsg(_socket, msg->getInternalMessage(), 0);

    return ret == 0;
}

}
}
