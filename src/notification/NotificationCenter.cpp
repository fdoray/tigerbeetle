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
#include "notification/NotificationCenter.hpp"

#include <assert.h>
#include <boost/regex.hpp>

#include "notification/NotificationSink.hpp"

#include <iostream>

namespace tibee
{
namespace notification
{

namespace
{

bool TokenMatch(const Token& token, const Token& search)
{
    assert(!token.isRegex());

    if (!search.isRegex())
        return token.token() == search.token();

    boost::regex searchBre;
    try {
        searchBre = search.token();
    } catch (const std::exception& ex) {
        return false;
    }

    return boost::regex_search(token.token(), searchBre);
}

}  // namespace

const char* NotificationCenter::kNotificationCenterServiceName = "notificationCenter";

NotificationCenter::NotificationCenter()
{
}

NotificationCenter::~NotificationCenter()
{
}

void NotificationCenter::AddObserver(const Path& path,
                                     const Callback& callback)
{
    assert(!path.empty());
    
    auto pathKey = _observerPaths.CreateNodeKey(path);
    
    if (_pathToCallbacks.size() <= pathKey.get())
        _pathToCallbacks.resize(pathKey.get() + 1);
    if (_pathToCallbacks[pathKey.get()].get() == nullptr)
        _pathToCallbacks[pathKey.get()].reset(new CallbackContainer);

    _pathToCallbacks[pathKey.get()]->push_back(callback);
}

const NotificationSink* NotificationCenter::GetSink(const Path& path)
{
    auto look = _pathToSinks.find(path);
    if (look != _pathToSinks.end())
        return look->second.get();

    // Create a new sink.
    CallbackContainers callbacks;
    FindCallbacks(path, 0, 0, &callbacks);

    auto sink = NotificationSink::UP {
        new NotificationSink { path, callbacks } };
    auto sinkPtr = sink.get();
    _pathToSinks[path] = std::move(sink);

    return sinkPtr;
}

void NotificationCenter::FindCallbacks(const Path& path,
                                       size_t pathIndex,
                                       keyed_tree::NodeKey node,
                                       CallbackContainers* callbacks)
{
    // Add the callbacks for |node|.
    if (pathIndex != 0 &&
        node.get() < _pathToCallbacks.size() &&
        _pathToCallbacks[node.get()])
    {
        callbacks->push_back(_pathToCallbacks[node.get()].get());
    }

    // End of the path.
    if (pathIndex >= path.size())
        return;

    // Add the callbacks for the children of |node|.
    auto it = _observerPaths.node_children_begin(node);
    auto it_end = _observerPaths.node_children_end(node);

    for (; it != it_end; ++it)
    {
        const auto& label = it->first;
        if (TokenMatch(path[pathIndex], label))
            FindCallbacks(path, pathIndex + 1, it->second, callbacks);
    }
}

}
}
