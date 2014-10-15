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
#ifndef _TIBEE_MQ_MESSAGEDECODER_HPP
#define _TIBEE_MQ_MESSAGEDECODER_HPP

#include <assert.h>
#include <boost/utility.hpp>
#include <zmq.hpp>

namespace tibee
{
namespace mq
{

/**
 * Message decoder.
 *
 * @author Francois Doray
 */
class MessageDecoder :
    boost::noncopyable
{
public:
    MessageDecoder(zmq::message_t* message);

    template<typename T>
    bool Read(T* val) {
        assert(val != nullptr);
        if (RemainingBytes() < sizeof(T))
            return false;
        *val = *reinterpret_cast<T*>(reinterpret_cast<char*>(_message->data()) + _pos);
        _pos += sizeof(T);
        return true;
    }

    bool ReadString(std::string* str, size_t length);

    size_t RemainingBytes() const {
        return _message->size() - _pos;
    }

private:
    zmq::message_t* _message;
    size_t _pos;
};

}
}

#endif // _TIBEE_MQ_MESSAGEDECODER_HPP
