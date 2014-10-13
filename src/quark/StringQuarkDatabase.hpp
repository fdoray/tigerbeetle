/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#ifndef _TIBEE_QUARK_STRINGQUARKDATABASE_HPP
#define _TIBEE_QUARK_STRINGQUARKDATABASE_HPP

#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

#include "quark/QuarkDatabase.hpp"

namespace tibee
{
namespace quark
{

class StringQuarkDatabase
    : boost::noncopyable
{
public:
    typedef std::unique_ptr<StringQuarkDatabase> UP;

    StringQuarkDatabase();

    /*
     * Inserts a value in the database if it's not already present and returns
     * its quark.
     *
     * @param value An immutable value to add to the database.
     * @returns The quark for the value.
     */
    const Quark& StrQuark(const std::string& str);
    Quark IntQuark(int value);

    /*
     * Returns the value of a quark.
     *
     * @param quark The quark of the value to retrieve.
     * @returns The value associated with the provided quark.
     */
    const std::string& String(const Quark& quark) const;

private:
    QuarkDatabase<std::string> _quarks;
};

}
}

#endif // _TIBEE_QUARK_STRINGQUARKDATABASE_HPP
