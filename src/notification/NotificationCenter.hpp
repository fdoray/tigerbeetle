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
#ifndef _TIBEE_NOTIFICATION_NOTIFICATIONCENTER_HPP
#define _TIBEE_NOTIFICATION_NOTIFICATIONCENTER_HPP

#include <boost/functional/hash.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "notification/NotificationKey.hpp"
#include "notification/Token.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace notification
{

// Forward declaration.
class NotificationSink;

typedef std::vector<Token> KeyPath;

/**
 * Notification center.
 *
 * @author Francois Doray
 */
class NotificationCenter {
public:
    friend class NotificationSink;

    typedef std::function<void (const KeyPath& path, const value::Value* value)>
        OnNotificationFunc;

    NotificationCenter();
    ~NotificationCenter();

    NotificationSink* GetSink(const KeyPath& path);

    void AddObserver(const KeyPath& path,
                     const OnNotificationFunc& function);

private:
    void PostNotification(NotificationKey key,
                          const KeyPath& path,
                          const value::Value* value);

    void EnsureKeyPath(const KeyPath& path);

    typedef std::unordered_map<KeyPath, NotificationKey, boost::hash<KeyPath>> Keys;
    Keys _keys;

    typedef std::unordered_map<KeyPath, NotificationSink*, boost::hash<KeyPath>> Sinks;
    Sinks _sinks;

    typedef std::vector<OnNotificationFunc> FunctionContainer;
    typedef std::vector<std::unique_ptr<FunctionContainer>> Functions;
    Functions _functions;
};

}
}

#endif // _TIBEE_NOTIFICATION_NOTIFICATIONCENTER_HPP
