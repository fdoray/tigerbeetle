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
#include "quark/DiskQuarkDatabase.hpp"

#include <fstream>
#include <iostream>
#include <vector>

#include "base/print.hpp"

namespace tibee
{
namespace quark
{

namespace
{

using base::tberror;
using base::tbendl;

const char kStrToQuarkSuffix[] = "-strtoquark";
const char kQuarkToStrSuffix[] = "-quarktostr";
const char kNextQuarkSuffix[] = "-nextquark";

const int kMaxIntQuark = 65535;

struct invalidquarkexception : std::exception {
    const char* what() const noexcept { return "Query with an invalid quark in disk quark database."; }
};

}  // namespace

DiskQuarkDatabase::DiskQuarkDatabase(const std::string& file)
    : _filename(file), _nextQuark(0)
{
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::DB* strToQuarkDb = nullptr;
    leveldb::Status status = leveldb::DB::Open(options, file + kStrToQuarkSuffix, &strToQuarkDb);
    _strToQuarkDisk.reset(strToQuarkDb);

    if (!status.ok())
        tberror() << "Error while opening str to quark disk database. " << status.ToString() << tbendl();

    leveldb::DB* quarkToStrDb = nullptr;
    status = leveldb::DB::Open(options, file + kQuarkToStrSuffix, &quarkToStrDb);
    _quarkToStrDisk.reset(quarkToStrDb);

    if (!status.ok())
        tberror() << "Error while opening quark to str disk database." << tbendl();

    std::ifstream nextQuarkFile(_filename + kNextQuarkSuffix);
    if (nextQuarkFile.good())
    {
        nextQuarkFile >> _nextQuark;
    }
    else
    {
        // Insert int quarks.
        for (int i = 0; i <= kMaxIntQuark; ++i)
            StrQuark(std::to_string(i));
    }
}

DiskQuarkDatabase::~DiskQuarkDatabase()
{
    std::ofstream nextQuarkFile(_filename + kNextQuarkSuffix);
    nextQuarkFile << _nextQuark;
}

const Quark& DiskQuarkDatabase::StrQuark(const std::string& str)
{
    auto look = _strToQuark.find(str);
    if (look != _strToQuark.end())
        return look->second;

    // Search in the leveldb database.
    std::string quark_str;
    leveldb::Status status = _strToQuarkDisk->Get(leveldb::ReadOptions(), str, &quark_str);

    if (status.ok())
    {
        Quark::quark_t quark_value = atoi(quark_str.c_str());
        Quark quark(quark_value);

        _strToQuark[str] = quark;
        _quarkToStr[quark] = str;

        return _strToQuark[str];
    }

    // Insert in the leveldb database.
    quark::Quark quark(_nextQuark);
    ++_nextQuark;

    quark_str = std::to_string(quark.get());

    _strToQuarkDisk->Put(leveldb::WriteOptions(), str, quark_str);
    _quarkToStrDisk->Put(leveldb::WriteOptions(), quark_str, str);

    _strToQuark[str] = quark;
    _quarkToStr[quark] = str;

    return _strToQuark[str];
}

Quark DiskQuarkDatabase::IntQuark(int value)
{
    if (value >= 0 && value <= kMaxIntQuark)
        return Quark(value);
    return StrQuark(std::to_string(value));
}

const std::string& DiskQuarkDatabase::String(const Quark& quark)
{
    auto look = _quarkToStr.find(quark);
    if (look != _quarkToStr.end())
        return look->second;

    // Search in the leveldb database.
    std::string quark_str = std::to_string(quark.get());
    std::string str;

    leveldb::Status status = _quarkToStrDisk->Get(leveldb::ReadOptions(), quark_str, &str);

    if (status.ok())
    {
        _strToQuark[str] = quark;
        _quarkToStr[quark] = str;

        return _quarkToStr[quark];
    }

    invalidquarkexception ex;
    throw ex;

    return _err;
}

}
}
