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

// The format of the generated file is:
// - uint32_t Number of quarks.
// Then, for each quark:
// - uint32_t Size of the string (no terminating null character).
// - char[] The string.

#include "state_blocks/QuarkDatabaseWriter.hpp"

#include <fstream>

namespace tibee
{
namespace state_blocks
{

bool WriteQuarkDatabase(const std::string filename,
                        const quark::StringQuarkDatabase& quarks)
{
    // Open the attribute tree file.
    std::ofstream dbFile;
    dbFile.open(filename, std::ios::out | std::ios::binary);

    // Write the number of elements.
    uint32_t numElements = quarks.size();
    dbFile.write(reinterpret_cast<const char*>(&numElements), sizeof(numElements));

    // Write the strings.
    for (const auto& str : quarks)
    {
        uint32_t size = str->size();
        dbFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
        dbFile.write(str->c_str(), size);
    }

    dbFile.close();

    return true;
}

}
}
