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
#ifndef _TIBEE_NOTIFICATION_PATH_HPP
#define _TIBEE_NOTIFICATION_PATH_HPP

#include <boost/functional/hash.hpp>
#include <vector>

#include "notification/Token.hpp"

namespace tibee
{
namespace notification
{

typedef std::vector<Token> Path;

}
}

namespace std {

template <>
struct hash<tibee::notification::Path> {
  size_t operator()(const tibee::notification::Path& path) const {
    boost::hash<tibee::notification::Path> keyHash;
    return keyHash(path);
  }
};

}  // namespace std

#endif // _TIBEE_NOTIFICATION_PATH_HPP
