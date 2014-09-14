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
#include <yajl_gen.h>

#include "rpc/AbstractJsonRpcMessageEncoder.hpp"

namespace tibee
{
namespace rpc
{

AbstractJsonRpcMessageEncoder::AbstractJsonRpcMessageEncoder() :
    _yajlGen {nullptr}
{
    _yajlGen = ::yajl_gen_alloc(nullptr);
}

AbstractJsonRpcMessageEncoder::~AbstractJsonRpcMessageEncoder()
{
    if (_yajlGen) {
        ::yajl_gen_free(_yajlGen);
    }
}

void AbstractJsonRpcMessageEncoder::resetGenerator()
{
    ::yajl_gen_reset(_yajlGen, nullptr);
    ::yajl_gen_clear(_yajlGen);
}

std::unique_ptr<std::string>
AbstractJsonRpcMessageEncoder::encodeRequest(const AbstractRpcRequest& request,
                                             const ObjectEncodeFunc& paramsEncodeFunc)
{
    this->resetGenerator();

    // open object
    ::yajl_gen_map_open(_yajlGen);

    // method
    const auto& method = request.getMethod();
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("method"), 6);
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>(method.c_str()), method.size());

    // ID
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("id"), 2);
    ::yajl_gen_integer(_yajlGen, request.getId());

    // parameters
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("params"), 6);
    ::yajl_gen_array_open(_yajlGen);

    if (!paramsEncodeFunc(request, _yajlGen)) {
        return nullptr;
    }

    ::yajl_gen_array_close(_yajlGen);

    // close object
    ::yajl_gen_map_close(_yajlGen);

    return this->getJsonStringFromBuffer();
}

std::unique_ptr<std::string>
AbstractJsonRpcMessageEncoder::encodeResponse(const AbstractRpcResponse& response,
                                              const ObjectEncodeFunc& resultEncodeFunc,
                                              const ObjectEncodeFunc& errorEncodeFunc)
{
    this->resetGenerator();

    // open object
    ::yajl_gen_map_open(_yajlGen);

    // ID
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("id"), 2);
    ::yajl_gen_integer(_yajlGen, response.getId());

    // result
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("result"), 6);

    if (!resultEncodeFunc(response, _yajlGen)) {
        return nullptr;
    }

    // error
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("error"), 5);

    if (!errorEncodeFunc(response, _yajlGen)) {
        return nullptr;
    }

    // close object
    ::yajl_gen_map_close(_yajlGen);

    return this->getJsonStringFromBuffer();
}

std::unique_ptr<std::string>
AbstractJsonRpcMessageEncoder::encodeNotification(const AbstractRpcNotification& notification,
                                                  const ObjectEncodeFunc& paramsEncodeFunc)
{
    this->resetGenerator();

    // open object
    ::yajl_gen_map_open(_yajlGen);

    // method
    const auto& method = notification.getMethod();
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("method"), 6);
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>(method.c_str()), method.size());

    // ID
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("id"), 2);
    ::yajl_gen_null(_yajlGen);

    // parameters
    ::yajl_gen_string(_yajlGen, reinterpret_cast<const unsigned char*>("params"), 6);
    ::yajl_gen_array_open(_yajlGen);

    if (!paramsEncodeFunc(notification, _yajlGen)) {
        return nullptr;
    }

    ::yajl_gen_array_close(_yajlGen);

    // close object
    ::yajl_gen_map_close(_yajlGen);

    return this->getJsonStringFromBuffer();
}

std::unique_ptr<std::string>
AbstractJsonRpcMessageEncoder::getJsonStringFromBuffer()
{
    // copy buffer
    const unsigned char* buf;
    std::size_t len;

    ::yajl_gen_get_buf(_yajlGen, std::addressof(buf), std::addressof(len));

    std::unique_ptr<std::string> str {
        new std::string {reinterpret_cast<const char*>(buf), len}
    };

    return str;
}

}
}
