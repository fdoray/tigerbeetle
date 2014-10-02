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

private:
    size_t _key;
};

}
}

#endif // _TIBEE_KEYEDTREE_NODEKEY_HPP
