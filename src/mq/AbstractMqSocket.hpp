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
#ifndef _TIBEE_MQ_ABSTRACTMQSOCKET_HPP
#define _TIBEE_MQ_ABSTRACTMQSOCKET_HPP

#include <memory>
#include <cstdint>
#include <boost/utility.hpp>

#include "mq/MqMessage.hpp"

namespace tibee
{
namespace mq
{

class MqContext;

/**
 * Abstract message queue socket. All concrete sockets must inherit
 * this class.
 *
 * Most socket operations are common to all socket types, so they are
 * implemented here.
 *
 * @author Philippe Proulx
 */
class AbstractMqSocket :
    boost::noncopyable
{
public:
    /**
     * Builds an abstract socket.
     *
     * Internal socket is set to \a nullptr initially.
     */
    AbstractMqSocket(MqContext* context, int socketType);

    virtual ~AbstractMqSocket() = 0;

    /**
     * Connects this socket to an endpoint using a given address.
     *
     * @param addr Address of endpoint to connect to
     * @returns    True if successful
     */
    bool connect(const std::string& addr);

    /**
     * Binds this socket to a created endpoint using a given address.
     *
     * @param addr Address of endpoint to create and bind to
     * @returns    True if successful
     */
    bool bind(const std::string& addr);

    /**
     * Closes this socket.
     *
     * @returns True if successful
     */
    bool close();

    /**
     * Receives a message from this socket.
     *
     * @returns Received message or \a nullptr if any error occured
     */
    MqMessage::UP recv();

    /**
     * Sends a message on this socket.
     *
     * @param msg Message to send
     * @returns   True if successful
     */
    bool send(MqMessage::UP msg);

protected:
    void* getInternalSocket()
    {
        return _socket;
    }

private:
    void* _socket;
};

}
}

#endif // _TIBEE_MQ_ABSTRACTMQSOCKET_HPP
