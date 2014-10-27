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
//  - uint32_t Number of attributes.
// Then, for each attribute:
//  - uint32_t Index of the parent node.
//  - uint32_t Quark to reach the node from the parent.
// The root attribute is not listed in the file. Its index is 0.

#include "state_blocks/AttributeTreeWriter.hpp"

#include <fstream>
#include <unordered_map>

namespace tibee
{
namespace state_blocks
{

namespace {

struct NodeInfo
{
    uint32_t parentKey;
    uint32_t quark;
};
typedef std::map<uint32_t, NodeInfo> NodeInfoMap;

bool CollectNodesInfo(state::AttributeKey key,
                      state::CurrentState* currentState,
                      NodeInfoMap* nodeInfoMap)
{
    auto it = currentState->attribute_children_begin(key);
    auto it_end = currentState->attribute_children_end(key);

    for (; it != it_end; ++it)
    {
        state::AttributeKey childKey = it->second;

        NodeInfo nodeInfo;
        nodeInfo.parentKey = key.get();
        nodeInfo.quark = it->first.get();

        (*nodeInfoMap)[childKey.get()] = nodeInfo;

        CollectNodesInfo(childKey, currentState, nodeInfoMap);
    }

    return true;
}

}  // namespace

bool WriteAttributeTree(const std::string filename,
                        state::CurrentState* currentState)
{
    // Collect information about the nodes.
    NodeInfoMap nodeInfoMap;
    CollectNodesInfo(keyed_tree::kRootNodeKey, currentState, &nodeInfoMap);

    // Open the attribute tree file.
    std::ofstream attributeTreeFile;
    attributeTreeFile.open(filename, std::ios::out | std::ios::binary);

    // Write the number of nodes in the tree.
    uint32_t numAttributes = nodeInfoMap.size();
    attributeTreeFile.write(reinterpret_cast<const char*>(&numAttributes), sizeof(numAttributes));

    // Write the attributes.
    for (const auto& nodeInfo : nodeInfoMap)
    {
        attributeTreeFile.write(reinterpret_cast<const char*>(&nodeInfo.second), sizeof(nodeInfo.second));
    }

    attributeTreeFile.close();

    return true;
}

}
}
