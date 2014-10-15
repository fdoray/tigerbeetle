/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "mq/MessageDecoder.hpp"

#include <iostream>

namespace tibee
{
namespace mq
{

MessageDecoder::MessageDecoder(zmq::message_t* message)
    : _message(message),
      _pos(0)
{
}

bool MessageDecoder::ReadString(std::string* str, size_t length)
{
    if (RemainingBytes() < length)
        return false;

    *str = std::string(reinterpret_cast<char*>(_message->data()) + _pos, length);
    _pos += length;

    return true;
}

}
}
