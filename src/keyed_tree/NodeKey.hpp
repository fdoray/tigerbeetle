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
#ifndef _TIBEE_KEYEDTREE_NODEKEY_HPP
#define _TIBEE_KEYEDTREE_NODEKEY_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <stddef.h>

namespace tibee
{
namespace keyed_tree
{

/**
 * Node key.
 *
 * @author Francois Doray
 */
class NodeKey
{
public:
    NodeKey() :
        _key(-1) {}
    NodeKey(size_t key) :
        _key(key) {}

    size_t get() const { return _key; }

    bool operator==(const NodeKey& other) const
    {
        return _key == other._key;
    }

    bool operator!=(const NodeKey& other) const
    {
        return _key != other._key;
    }

    size_t hash() const {
        return boost::hash_value(_key);
    }

private:
    size_t _key;
};

}  // namespace keyed_tree
}  // namespace tibee

namespace std {

template <>
struct hash<tibee::keyed_tree::NodeKey> {
  size_t operator()(const tibee::keyed_tree::NodeKey& key) const {
    return key.hash();
  }
};

}  // namespace std

#endif // _TIBEE_KEYEDTREE_NODEKEY_HPP
