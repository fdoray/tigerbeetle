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
#include "history/QuarkDatabaseReader.hpp"

#include <fstream>

namespace tibee
{
namespace history
{

bool ReadQuarkDatabase(const boost::filesystem::path& filename,
                       quark::StringQuarkDatabase* quarks)
{
    std::ifstream in(filename.string(), std::ios::in | std::ios::binary);

    // Read number of elements.
    uint32_t numElements = 0;
    in.read(reinterpret_cast<char*>(&numElements), sizeof(numElements));

    // Read elements.
    std::vector<char> buffer;
    for (size_t i = 0; i < numElements; ++i)
    {
        // Read string length.
        uint32_t length = 0;
        in.read(reinterpret_cast<char*>(&length), sizeof(length));

        // Read string.
        if (buffer.size() < length)
            buffer.resize(length);
        in.read(buffer.data(), length);

        // Insert in the quark database.
        std::string str(buffer.data(), length);
        quarks->StrQuark(str);
    }

    return true;
}

}
}
