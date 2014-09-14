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
#ifndef _TIBEE_MQ_MQMESSAGE_HPP
#define _TIBEE_MQ_MQMESSAGE_HPP

#include <boost/utility.hpp>
#include <cstddef>
#include <memory>
#include <zmq.h>

namespace tibee
{
namespace mq
{

/**
 * Message queue message. This is the atomic element sent and received
 * on message queue sockets.
 *
 * @author Philippe Proulx
 */
class MqMessage :
    boost::noncopyable
{
    friend class AbstractMqSocket;

public:
    typedef std::shared_ptr<MqMessage> SP;
    typedef std::unique_ptr<MqMessage> UP;

public:
    /**
     * Builds a message with a copy of user data.
     *
     * @param data User data
     * @param size Size of data in bytes
     */
    MqMessage(const void* data, std::size_t size);

    ~MqMessage();

    /**
     * Returns the message data size in bytes.
     *
     * @returns Contained data size in bytes
     */
    std::size_t size();

    /**
     * Returns a pointer to the contained data.
     *
     * @returns Pointer to contained data.
     */
    const void* data();

private:
    MqMessage();

    ::zmq_msg_t* getInternalMessage()
    {
        return std::addressof(_msg);
    }

private:
    ::zmq_msg_t _msg;
};

}
}

#endif // _TIBEE_MQ_MQMESSAGE_HPP
