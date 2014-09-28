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

#include "notification/NotificationSink.hpp"

namespace tibee
{
namespace notification
{

NotificationCenter::NotificationCenter()
{
}

NotificationCenter::~NotificationCenter()
{
    for (const auto& sink : _sinks) {
        delete sink.second;
    }
}

NotificationSink* NotificationCenter::GetSink(const KeyPath& path)
{
    EnsureKeyPath(path);
    return _sinks[path];
}

void NotificationCenter::RegisterObserver(const KeyPath& path,
                                          const OnNotificationFunc& function)
{
    EnsureKeyPath(path);
    NotificationKey key = _keys[path];
    _functions[key.get()]->push_back(function);
}

void NotificationCenter::PostNotification(NotificationKey key,
                                          const KeyPath& path,
                                          const value::Value* value)
{
    const auto& functions = *_functions[key.get()];
    for (const auto& function : functions) {
        function(path, value);
    }
}

void NotificationCenter::EnsureKeyPath(const KeyPath& path)
{
    assert(!path.empty());

    // Make sure that the sinks for all the subpaths exist.
    std::vector<NotificationKey> keys_so_far;
    for (size_t i = 0; i < path.size(); ++i) {
        KeyPath partial_path(path.begin(), path.begin() + i + 1);

        auto look = _keys.find(partial_path);
        if (look == _keys.end()) {
            // Generate a key.
            NotificationKey key(_functions.size());
            keys_so_far.push_back(key);
            _keys[partial_path] = key;
            _sinks[partial_path] = new NotificationSink {
                this,
                partial_path,
                keys_so_far
            };

            // Create a container for the functions that handle this path.
            std::unique_ptr<FunctionContainer> functions(new FunctionContainer());
            _functions.push_back(std::move(functions));
        } else {
            keys_so_far.push_back(look->second);
        }
    }
}

}
}
