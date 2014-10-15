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
#ifndef _TIBEE_MQ_MESSAGEENCODER_HPP
#define _TIBEE_MQ_MESSAGEENCODER_HPP

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
class MessageEncoder :
    boost::noncopyable
{
public:
    MessageEncoder();

    template<typename T>
    void Write(const T& val) {
        _buffer.resize(_buffer.size() + sizeof(val));
        memcpy(_buffer.data() + _buffer.size() - sizeof(val), &val, sizeof(val));
    }

    void WriteString(const std::string& str);

    size_t size() const { return _buffer.size(); }
    const char* data() const { return _buffer.data(); }

private:
    std::vector<char> _buffer;
};

}
}

#endif // _TIBEE_MQ_MESSAGEENCODER_HPP
