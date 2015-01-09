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
#ifndef _TIBEE_QUARK_DISKQUARKDATABASE_HPP
#define _TIBEE_QUARK_DISKQUARKDATABASE_HPP

#include <boost/noncopyable.hpp>
#include <leveldb/db.h>
#include <memory>
#include <string>
#include <unordered_map>

#include "quark/Quark.hpp"

namespace tibee
{
namespace quark
{

class DiskQuarkDatabase
    : boost::noncopyable
{
public:
    typedef std::unique_ptr<DiskQuarkDatabase> UP;

    DiskQuarkDatabase(const std::string& file);
    ~DiskQuarkDatabase();

    /*
     * Inserts a value in the database if it's not already present and returns
     * its quark.
     *
     * @param value An immutable value to add to the database.
     * @returns The quark for the value.
     */
    const Quark& StrQuark(const std::string& str);

    /*
     * Returns the value of a quark.
     *
     * @param quark The quark of the value to retrieve.
     * @returns The value associated with the provided quark.
     */
    const std::string& String(const Quark& quark);

private:
    // Filename.
    std::string _filename;

    // String -> Quark on disk.
    std::unique_ptr<leveldb::DB> _strToQuarkDisk;

    // Quark -> String in memory.
    std::unique_ptr<leveldb::DB> _quarkToStrDisk;

    // String -> Quark
    std::unordered_map<std::string, Quark> _strToQuark;

    // Quark -> String
    std::unordered_map<Quark, std::string> _quarkToStr;

    // Next quark.
    Quark::quark_t _nextQuark;

    // Error string.
    std::string _err;
};

}
}

#endif // _TIBEE_QUARK_DISKQUARKDATABASE_HPP
