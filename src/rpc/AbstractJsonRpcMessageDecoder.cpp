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
#include <yajl_parse.h>

#include "rpc/AbstractJsonRpcMessageDecoder.hpp"

namespace tibee
{
namespace rpc
{

AbstractJsonRpcMessageDecoder::AbstractJsonRpcMessageDecoder() :
    _yajlHandle {nullptr}
{
    static const ::yajl_callbacks callbacks = {
        processNullCb,
        processBooleanCb,
        processIntegerCb,
        processDoubleCb,
        processNumberCb,
        processStringCb,
        processStartMapCb,
        processMapKeyCb,
        processEndMapCb,
        processStartArrayCb,
        processEndArrayCb,
    };

    _yajlHandle = ::yajl_alloc(std::addressof(callbacks), nullptr,
                               static_cast<void*>(this));
}

AbstractJsonRpcMessageDecoder::~AbstractJsonRpcMessageDecoder()
{
    if (_yajlHandle) {
        ::yajl_free(_yajlHandle);
    }
}

int AbstractJsonRpcMessageDecoder::processNullCb(void* ctx)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processNull();

    return 1;
}

int AbstractJsonRpcMessageDecoder::processBooleanCb(void* ctx, int value)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processBoolean(value != 0);

    return 1;
}

int AbstractJsonRpcMessageDecoder::processIntegerCb(void* ctx, long long value)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processInteger(value != 0);

    return 1;
}

int AbstractJsonRpcMessageDecoder::processDoubleCb(void* ctx, double value)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processDouble(value);

    return 1;
}

int AbstractJsonRpcMessageDecoder::processNumberCb(void* ctx, const char* number, std::size_t len)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processNumber(number, len);

    return 1;
}

int AbstractJsonRpcMessageDecoder::processStringCb(void* ctx, const unsigned char* value, std::size_t len)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processString(reinterpret_cast<const char*>(value), len);

    return 1;
}

int AbstractJsonRpcMessageDecoder::processStartMapCb(void* ctx)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processStartMap();

    return 1;
}

int AbstractJsonRpcMessageDecoder::processMapKeyCb(void* ctx, const unsigned char* key, std::size_t len)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processMapKey(reinterpret_cast<const char*>(key), len);

    return 1;
}

int AbstractJsonRpcMessageDecoder::processEndMapCb(void* ctx)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processEndMap();

    return 1;
}

int AbstractJsonRpcMessageDecoder::processStartArrayCb(void* ctx)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processStartArray();

    return 1;
}

int AbstractJsonRpcMessageDecoder::processEndArrayCb(void* ctx)
{
    AbstractJsonRpcMessageDecoder* decoder = static_cast<AbstractJsonRpcMessageDecoder*>(ctx);

    decoder->processEndArray();

    return 1;
}

bool AbstractJsonRpcMessageDecoder::parse(const char* json, std::size_t len)
{
    auto ret = ::yajl_parse(_yajlHandle,
                            reinterpret_cast<const unsigned char*>(json),
                            len);

    return ret == ::yajl_status_ok;
}

}
}
