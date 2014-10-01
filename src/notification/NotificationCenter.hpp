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

#include <string>
#include <unordered_map>
#include <vector>

#include "keyed_tree/KeyedTree.hpp"
#include "notification/Callback.hpp"
#include "notification/Path.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Token.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace notification
{

/**
 * Notification center.
 *
 * @author Francois Doray
 */
class NotificationCenter {
public:
    friend class NotificationSink;

    NotificationCenter();
    ~NotificationCenter();

    void AddObserver(const Path& path,
                     const Callback& function);

    const NotificationSink* GetSink(const Path& path);

private:
    void FindCallbacks(const Path& path,
                       size_t pathIndex,
                       keyed_tree::NodeKey node,
                       CallbackContainers* containers);
    typedef keyed_tree::KeyedTree<Token> ObserverPaths;
    ObserverPaths _observerPaths;

    typedef std::vector<std::unique_ptr<CallbackContainer>> PathToCallbacks;
    PathToCallbacks _pathToCallbacks;

    typedef std::unordered_map<Path, NotificationSink::UP> PathToSinks;
    PathToSinks _pathToSinks;
};

}
}

#endif // _TIBEE_NOTIFICATION_NOTIFICATIONCENTER_HPP
