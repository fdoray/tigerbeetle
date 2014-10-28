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
#include "history/AttributeTreeReader.hpp"

#include <fstream>

#include "quark/Quark.hpp"
#include "state/AttributeKey.hpp"
#include "state/AttributePath.hpp"

namespace tibee
{
namespace history
{

bool ReadAttributeTree(const boost::filesystem::path& filename,
                       state::AttributeTree* attributeTree)
{
    std::ifstream in(filename.string(), std::ios::in | std::ios::binary);

    // Read number of attributes.
    uint32_t numAttributes = 0;
    in.read(reinterpret_cast<char*>(&numAttributes), sizeof(numAttributes));

    // Read the attributes.
    for (size_t i = 1; i < numAttributes; ++i)
    {
        uint32_t parentKey = 0;
        in.read(reinterpret_cast<char*>(&parentKey), sizeof(parentKey));
        uint32_t quark = 0;
        in.read(reinterpret_cast<char*>(&quark), sizeof(quark));

        attributeTree->CreateNodeKey(
            state::AttributeKey(parentKey),
            state::AttributePath {quark::Quark(quark)});
    }

    return true;
}

}
}
