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
#include "quark/StringQuarkDatabase.hpp"

namespace tibee
{
namespace quark
{

namespace
{
const int kMaxIntQuark = 65535;
}  // namespace

StringQuarkDatabase::StringQuarkDatabase()
{
    for (int i = 0; i <= kMaxIntQuark; ++i)
        StrQuark(std::to_string(i));
}

const Quark& StringQuarkDatabase::StrQuark(const std::string& str)
{
    return _quarks.Insert(str);
}

Quark StringQuarkDatabase::IntQuark(int value)
{
    if (value >= 0 && value <= kMaxIntQuark)
        return Quark(value);
    return StrQuark(std::to_string(value));
}

const std::string& StringQuarkDatabase::String(const Quark& quark) const
{
    return _quarks.ValueOf(quark);
}

}
}
