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
#ifndef _TIBEE_COMMON_QUARKDATABASE_HPP
#define _TIBEE_COMMON_QUARKDATABASE_HPP

#include <assert.h>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <memory>
#include <utility>
#include <vector>

#include <common/quark/Quark.hpp>

namespace tibee
{
namespace common
{

/**
 * Quark database.
 *
 * Keeps track of the mapping between quarks and immutable objects.
 * Since a single copy of each immutable object is shared between
 * multiple users, memory usage of the application is reduced.
 *
 * @author Francois Doray
 */
template <typename T>
class QuarkDatabase
    : boost::noncopyable
{
public:
    QuarkDatabase();

    /*
     * Inserts a value in the databse if it's not already present and returns
     * its quark.
     *
     * @param value An immutable value to add to the database.
     * @returns The quark for the value.
     */
    const Quark& Insert(const T& value);

    /*
     * Returns the value of a quark.
     *
     * @param quark The quark of the value to retrieve.
     * @returns The value associated with the provided quark.
     */
    const T& ValueOf(const Quark& quark) const;

private:
    typedef typename boost::unordered_map<const T, Quark> ValuesToKeysMap;
    typedef typename std::vector<const T*> KeysToValuesVector;

    ValuesToKeysMap values_map_;
    KeysToValuesVector keys_;
};

template <typename T>
QuarkDatabase<T>::QuarkDatabase() {
}

template <typename T>
const Quark& QuarkDatabase<T>::Insert(const T& value) {
    // Check whether this value already exists.
    auto look = values_map_.find(value);
    if (look != values_map_.end())
        return look->second;

    // Insert an instance of this value.
    auto inserted = values_map_.insert(
        std::make_pair(value, Quark(keys_.size())));
    assert(inserted.second);

    // Add the pointer to |keys_|.
    auto place = inserted.first;
    const Quark& new_quark = place->second;
    const T& new_value = place->first;
    keys_.push_back(&new_value);

    return new_quark;
}

template <typename T>
const T& QuarkDatabase<T>::ValueOf(const Quark& quark) const {
    return *keys_.at(quark.get());
}

}  // namespace common
}  // namespace tibee

#endif // _TIBEE_COMMON_QUARKDATABASE_HPP