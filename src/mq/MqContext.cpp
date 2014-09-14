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
#include <cstdint>
#include <zmq.h>
#include <memory>

#include "mq/MqContext.hpp"
#include "mq/PublishMqSocket.hpp"
#include "mq/RequestMqSocket.hpp"
#include "mq/ReplyMqSocket.hpp"
#include "mq/SubscribeMqSocket.hpp"
#include "mq/ex/MqContext.hpp"

namespace tibee
{
namespace mq
{

MqContext::MqContext(std::size_t ioThreads) :
    _context {nullptr}
{
    _context = ::zmq_init(ioThreads);

    if (!_context) {
        throw ex::MqContext {"cannot create message queue context"};
    }
}

MqContext::~MqContext()
{
    if (_context) {
        ::zmq_term(_context);
    }
}

std::unique_ptr<RequestMqSocket> MqContext::createRequestSocket()
{
    return std::unique_ptr<RequestMqSocket> {new RequestMqSocket {this}};
}

std::unique_ptr<ReplyMqSocket> MqContext::createReplySocket()
{
    return std::unique_ptr<ReplyMqSocket> {new ReplyMqSocket {this}};
}

std::unique_ptr<PublishMqSocket> MqContext::createPublishSocket()
{
    return std::unique_ptr<PublishMqSocket> {new PublishMqSocket {this}};
}

std::unique_ptr<SubscribeMqSocket> MqContext::createSubscribeSocket()
{
    return std::unique_ptr<SubscribeMqSocket> {new SubscribeMqSocket {this}};
}

}
}
