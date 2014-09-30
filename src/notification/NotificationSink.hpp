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
#ifndef _TIBEE_NOTIFICATION_NOTIFICATIONSINK_HPP
#define _TIBEE_NOTIFICATION_NOTIFICATIONSINK_HPP

#include <vector>

#include "notification/NotificationCenter.hpp"
#include "notification/NotificationKey.hpp"

namespace tibee
{

namespace value
{
// Forward declaration.
class Value;
}

namespace notification
{

// Forward declaration.
class NotificationCenter;

/**
 * Notification sink.
 *
 * @author Francois Doray
 */
class NotificationSink {
public:
    friend class NotificationCenter;

    void PostNotification(const value::Value* value);

private:
    typedef std::vector<NotificationKey> NotificationKeyContainer;

    NotificationSink(NotificationCenter* notificationCenter,
                     const KeyPath& path,
                     const NotificationKeyContainer& keys);
    ~NotificationSink();

    NotificationCenter* _notificationCenter;
    KeyPath _path;
    NotificationKeyContainer _keys;
};

}
}

#endif // _TIBEE_NOTIFICATION_NOTIFICATIONSINK_HPP
