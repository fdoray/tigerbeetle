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
#include <cstring>
#include <zmq.h>

#include "mq/MqMessage.hpp"
#include "mq/ex/MqMessage.hpp"

namespace tibee
{
namespace mq
{

MqMessage::MqMessage()
{
    auto ret = ::zmq_msg_init(std::addressof(_msg));

    if (ret < 0) {
        throw ex::MqMessage {"cannot create message queue message"};
    }
}

MqMessage::MqMessage(const void* data, std::size_t size)
{
    auto ret = ::zmq_msg_init_size(std::addressof(_msg), size);

    if (ret < 0) {
        throw ex::MqMessage {"cannot create message queue message"};
    }

    std::memcpy(::zmq_msg_data(std::addressof(_msg)), data, size);
}

MqMessage::~MqMessage()
{
    // TODO: verify if safe to call with a nullified message
    ::zmq_msg_close(std::addressof(_msg));
}

std::size_t MqMessage::size()
{
    return ::zmq_msg_size(std::addressof(_msg));
}

const void* MqMessage::data()
{
    return ::zmq_msg_data(std::addressof(_msg));
}

}
}
