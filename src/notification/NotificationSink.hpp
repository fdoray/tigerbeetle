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

#include <memory>
#include <vector>

#include "notification/Callback.hpp"
#include "notification/Path.hpp"

namespace tibee
{

namespace value
{
// Forward declaration.
class Value;
}

namespace notification
{

/**
 * Notification sink.
 *
 * @author Francois Doray
 */
class NotificationSink {
public:
    typedef std::unique_ptr<NotificationSink> UP;
    friend class NotificationCenter;
    friend std::unique_ptr<NotificationSink>::deleter_type;

    void PostNotification(const value::Value* value) const;

private:
    NotificationSink(const Path& path,
                     const CallbackContainers& callbacks);
    ~NotificationSink();

    Path _path;
    CallbackContainers _callbacks;
};

}
}

#endif // _TIBEE_NOTIFICATION_NOTIFICATIONSINK_HPP
