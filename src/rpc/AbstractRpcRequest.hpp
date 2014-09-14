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
#ifndef _TIBEE_RPC_ABSTRACTRPCREQUEST_HPP
#define _TIBEE_RPC_ABSTRACTRPCREQUEST_HPP

#include <string>

#include "base/BasicTypes.hpp"
#include "rpc/AbstractRpcMessage.hpp"
#include "rpc/RpcMessageType.hpp"

namespace tibee
{
namespace rpc
{

/**
 * Abstract RPC request. All RPC requests must inherit this class.
 *
 * @author Philippe Proulx
 */
class AbstractRpcRequest :
    public AbstractRpcMessage
{
public:
    /**
     * Builds an abstract RPC request.
     *
     * @param method RPC request method name
     */
    AbstractRpcRequest(const std::string& method);

    virtual ~AbstractRpcRequest();

    /**
     * Returns the RPC request method name.
     *
     * @returns RPC request method name
     */
    const std::string& getMethod() const
    {
        return _method;
    }

    /**
     * Sets the RPC request ID.
     *
     * @param id RPC request ID
     */
    void setId(rpc_msg_id_t id)
    {
        _id = id;
    }

    /**
     * Returns the RPC request ID.
     *
     * @returns RPC request ID.
     */
    rpc_msg_id_t getId() const
    {
        return _id;
    }

private:
    rpc_msg_id_t _id;
    std::string _method;
};

}
}

#endif // _TIBEE_RPC_ABSTRACTRPCREQUEST_HPP
