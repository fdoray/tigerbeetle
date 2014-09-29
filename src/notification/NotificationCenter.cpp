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

bool PathsMatch(const NotificationCenter::KeyPath& path,
                const NotificationCenter::KeyPath& match_path)
{
    if (path.size() != match_path.size())
        return false;

    auto path_it = path.begin();
    auto match_path_it = match_path.begin();
    for (; path_it != path.end(); ++path_it, ++match_path_it)
    {
        if (path_it->isRegex())
        {
            boost::regex tokenBre;
            try {
                tokenBre = path_it->token();
            } catch (const std::exception& ex) {
                return false;
            }

            if (!boost::regex_search(match_path_it->token(), tokenBre)) {
                return false;
            }
        }
        else
        {
            if (path_it->token() != match_path_it->token())
                return false;
        }
    }

    return true;
}

}  // namespace

NotificationCenter::NotificationCenter()
{
}

NotificationCenter::~NotificationCenter()
{
    for (const auto& sink : _sinks) {
        delete sink.second;
    }
}

NotificationSink* NotificationCenter::GetNotificationSink(const KeyPath& path)
{
    EnsureKeyPath(path);
    return _sinks[path];
}

void NotificationCenter::RegisterNotificationObserver(const KeyPath& path,
                                                      const OnNotificationFunc& function)
{
    assert(!path.empty());

    // Find the index of the first regex token.
    size_t first_regex_index = 0;
    while (first_regex_index < path.size()) {
        if (!path[first_regex_index].isRegex())
            ++first_regex_index;
        else
            break;
    }

    if (first_regex_index == path.size())
    {
        // Register a function for a path without regex.
        auto key = _keys.find(path);
        if (key == _keys.end())
            return;
        _functions[key->second.get()]->push_back(function);
    }
    else
    {
        // Register a function for a path with regex.
        // TODO: Improve the complexity of this algorithm.
        // TODO: Precompile the regex.
        for (const auto& match_path : _keys)
        {
            if (PathsMatch(path, match_path.first))
            {
                _functions[match_path.second.get()]->push_back(function);
            }
        }
    }
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

    if (_keys.find(path) != _keys.end())
        return;

    // Make sure that the sinks for all the subpaths exist.
    std::vector<NotificationKey> keys_so_far;
    for (size_t i = 0; i < path.size(); ++i) {
        assert(!path[i].isRegex());

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
