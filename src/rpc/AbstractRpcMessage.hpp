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
#ifndef _TIBEE_RPC_ABSTRACTRPCMESSAGE_HPP
#define _TIBEE_RPC_ABSTRACTRPCMESSAGE_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rpc/RpcMessageType.hpp"

namespace tibee
{
namespace rpc
{

/// RPC message ID type.
typedef std::uint32_t rpc_msg_id_t;

/**
 * Abstract RPC message. All RPC messages must inherit this class.
 *
 * @author Philippe Proulx
 */
class AbstractRpcMessage
{
public:
    /**
     * Builds an abstract RPC message of a given type.
     *
     * @param type Concrete RPC message type
     */
    AbstractRpcMessage(RpcMessageType type) :
        _type {type}
    {
    }

    virtual ~AbstractRpcMessage() = 0;

    /**
     * Returns the type of this message.
     *
     * @returns Message type
     */
    RpcMessageType getType() const
    {
        return _type;
    }

    /**
     * Returns whether or not this message is a request.
     *
     * @returns True if this message is a request
     */
    bool isRequest() const
    {
        return _type == RpcMessageType::REQUEST;
    }

    /**
     * Returns whether or not this message is a response.
     *
     * @returns True if this message is a response
     */
    bool isResponse() const
    {
        return _type == RpcMessageType::RESPONSE;
    }

    /**
     * Returns whether or not this message is a notification.
     *
     * @returns True if this message is a notification
     */
    bool isNotification() const
    {
        return _type == RpcMessageType::NOTIFICATION;
    }

private:
    RpcMessageType _type;
};

}
}

#endif // _TIBEE_RPC_ABSTRACTRPCMESSAGE_HPP
