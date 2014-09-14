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
#ifndef _TIBEE_RPC_ABSTRACTJSONRPCMESSAGEENCODER_HPP
#define _TIBEE_RPC_ABSTRACTJSONRPCMESSAGEENCODER_HPP

#include <yajl_gen.h>
#include <vector>
#include <cstdint>
#include <memory>
#include <cstring>
#include <functional>

#include "rpc/AbstractRpcMessage.hpp"
#include "rpc/AbstractRpcRequest.hpp"
#include "rpc/AbstractRpcResponse.hpp"
#include "rpc/AbstractRpcNotification.hpp"

#define TIBEE_DEF_YAJL_STR(name, str) \
    static const unsigned char* name = reinterpret_cast<const unsigned char*>(str); \
    static const std::size_t name##_LEN = std::strlen(reinterpret_cast<const char*>(name))

namespace tibee
{
namespace rpc
{

/**
 * Abstract JSON-RPC message encoder. All concrete JSON-RPC encoders
 * must inherit this class.
 *
 * This class provides JSON utilities.
 *
 * @author Philippe Proulx
 */
class AbstractJsonRpcMessageEncoder
{
public:
    /**
     * Builds an abstract JSON-RPC message encoder.
     */
    AbstractJsonRpcMessageEncoder();

    virtual ~AbstractJsonRpcMessageEncoder();

protected:
    typedef std::function<bool (const AbstractRpcMessage&, ::yajl_gen)> ObjectEncodeFunc;

protected:
    /**
     * Encodes an RPC request as JSON-RPC.
     *
     * @param request          RPC request to encode
     * @param paramsEncodeFunc Encode function to use for parameters
     * @returns                Encoded request or \a nullptr if an error occured
     */
    std::unique_ptr<std::string> encodeRequest(const AbstractRpcRequest& request,
                                               const ObjectEncodeFunc& paramsEncodeFunc);

    /**
     * Encodes an RPC response as JSON-RPC.
     *
     * @param response         RPC response to encode
     * @param resultEncodeFunc Encode function to use for result
     * @param errorEncodeFunc  Encode function to use for error
     * @returns                Encoded response or \a nullptr if an error occured
     */
    std::unique_ptr<std::string> encodeResponse(const AbstractRpcResponse& response,
                                                const ObjectEncodeFunc& resultEncodeFunc,
                                                const ObjectEncodeFunc& errorEncodeFunc);

    /**
     * Encodes an RPC notification as JSON-RPC.
     *
     * @param notification     RPC notification to encode
     * @param paramsEncodeFunc Encode function to use for parameters
     * @returns                Encoded notification or \a nullptr if an error occured
     */
    std::unique_ptr<std::string> encodeNotification(const AbstractRpcNotification& notification,
                                                    const ObjectEncodeFunc& paramsEncodeFunc);

private:
    void resetGenerator();
    std::unique_ptr<std::string> getJsonStringFromBuffer();

private:
    ::yajl_gen _yajlGen;
};

}
}

#endif // _TIBEE_RPC_ABSTRACTJSONRPCMESSAGEENCODER_HPP
