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
#ifndef _TIBEE_RPC_ABSTRACTJSONRPCMESSAGEDECODER_HPP
#define _TIBEE_RPC_ABSTRACTJSONRPCMESSAGEDECODER_HPP

#include <yajl_parse.h>
#include <vector>
#include <cstdint>
#include <memory>
#include <functional>

namespace tibee
{
namespace rpc
{

/**
 * Abstract JSON-RPC message decoder. All concrete JSON-RPC decoders
 * must inherit this class.
 *
 * A JSON-RPC message decoder will typically have one method per
 * possible message, each one setting a specific object state (object
 * currently decoded, stack of JSON states, etc.) and call parse().
 *
 * @author Philippe Proulx
 */
class AbstractJsonRpcMessageDecoder
{
public:
    /**
     * Builds an abstract JSON-RPC message decoder.
     */
    AbstractJsonRpcMessageDecoder();

    virtual ~AbstractJsonRpcMessageDecoder();

private:
    virtual void processNull() = 0;
    virtual void processBoolean(bool value) = 0;
    virtual void processInteger(long long value) = 0;
    virtual void processDouble(double value) = 0;
    virtual void processNumber(const char* number, std::size_t len) = 0;
    virtual void processString(const char* value, std::size_t len) = 0;
    virtual void processStartMap() = 0;
    virtual void processMapKey(const char* key, std::size_t len) = 0;
    virtual void processEndMap() = 0;
    virtual void processStartArray() = 0;
    virtual void processEndArray() = 0;

    /**
     * Parses a JSON string, calling appropriate callbacks above when
     * meeting new tokens.
     *
     * @param json JSON string to parse
     * @param len  JSON string length (bytes)
     * @returns    True if successfully decoded
     */
    bool parse(const char* json, std::size_t len);

    static int processNullCb(void* ctx);
    static int processBooleanCb(void* ctx, int value);
    static int processIntegerCb(void* ctx, long long value);
    static int processDoubleCb(void* ctx, double value);
    static int processNumberCb(void* ctx, const char* number, std::size_t len);
    static int processStringCb(void* ctx, const unsigned char* value, std::size_t len);
    static int processStartMapCb(void* ctx);
    static int processMapKeyCb(void* ctx, const unsigned char* key, std::size_t len);
    static int processEndMapCb(void* ctx);
    static int processStartArrayCb(void* ctx);
    static int processEndArrayCb(void* ctx);

private:
    ::yajl_handle _yajlHandle;
};

}
}

#endif // _TIBEE_RPC_ABSTRACTJSONRPCMESSAGEDECODER_HPP
