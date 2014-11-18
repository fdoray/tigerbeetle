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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.    If not, see <http://www.gnu.org/licenses/>.
 */
#include "execution/Matcher.hpp"

#include <algorithm>
#include <unordered_map>

#include "base/Constants.hpp"

namespace tibee {
namespace execution {

namespace {

// Stores the minimum cost to match 2 subtrees.
struct SubtreeCost {
    SubtreeCost()
        : cost(0), next_match(), is_matching(false) {
    }

    SubtreeCost(uint64_t cost, const NodePair& next_match, bool is_matching)
        : cost(cost), next_match(next_match), is_matching(is_matching) {
    }

    // The minimum cost to match the subtrees.
    uint64_t cost;

    // The first pair of nodes that match in a preorder traversal of the subtrees,
    // after the root.
    NodePair next_match;

    // Indicates whether the roots of the subtrees match.
    bool is_matching;
};

// Stores information about a node.
struct NodeInfo {
    NodeInfo(NodeId node_id, uint32_t depth)
        : node_id(node_id), depth(depth), next_depth_drop(-1) {
    }

    // The identifier of the node.
    NodeId node_id;

    // The depth of the node. The depth of the root is 0.
    size_t depth;

    // The index of the next node with a smaller depth in a preorder traversal
    // of the tree, or the number of nodes in the graph if none.
    size_t next_depth_drop;
};
typedef std::vector<NodeInfo> NodeInfoVector;

void GeneratePreorderTraversal(const Graph& graph,
                               NodeInfoVector* node_info_vector) {
    assert(node_info_vector);
    assert(node_info_vector->empty());

    auto it = graph.preorder_begin();
    auto end = graph.preorder_end();

    // Preorder traversal.
    for (; it != end; ++it) {
        node_info_vector->push_back(NodeInfo(it->id(), it.Depth()));
    }

    // Get the index of the next node with a smaller depth for each node.
    std::vector<size_t> depth_to_node;

    for (int64_t i = node_info_vector->size() - 1; i >= 0; --i) {
        size_t depth = node_info_vector->at(i).depth + 1;

        if (depth_to_node.size() <= depth) {
            depth_to_node.resize(depth + 1, node_info_vector->size());
        }

        // Find the next node with a depth smaller than |depth|.
        size_t next_depth_drop = node_info_vector->size();
        for (int64_t j = depth - 1; j >= 0; --j) {
            if (depth_to_node[j] < next_depth_drop)
                next_depth_drop = depth_to_node[j];
            else
                depth_to_node[j] = next_depth_drop;
        }
        node_info_vector->at(i).next_depth_drop = next_depth_drop;

        // Mark the current node as the next with depth |depth|.
        depth_to_node[depth] = i;
    }
}

typedef std::unordered_map<NodePair, SubtreeCost> MatchCache;

uint64_t MatchInternal(const NodeInfoVector& graph_a_traversal,
                       const NodeInfoVector& graph_b_traversal,
                       const MatchNodesCostFunc& match_cost_func,
                       uint64_t skip_cost,
                       size_t pos_a,
                       size_t pos_b,
                       NodePair* next_match,
                       MatchCache* cache) {
    // Reached the end of a graph: insert all the nodes from the other graph.
    if (pos_a >= graph_a_traversal.size()) {
        return (graph_b_traversal.size() - pos_b) * skip_cost;
    } else if (pos_b >= graph_b_traversal.size()) {
        return (graph_a_traversal.size() - pos_a) * skip_cost;
    }

    int64_t diff = static_cast<int64_t>(pos_a) - static_cast<int64_t>(pos_b);
    if (std::max(diff, -diff) > 6000)
        return kHugeCost;

    // Look if the match is in the cache.
    NodePair current_pair(graph_a_traversal[pos_a].node_id,
                          graph_b_traversal[pos_b].node_id);

    auto look = cache->find(current_pair);
    if (look != cache->end()) {
        if (look->second.is_matching)
            *next_match = current_pair;
        else
            *next_match = look->second.next_match;

        return look->second.cost;
    }

    // Match the 2 nodes.
    uint64_t match_cost = kHugeCost;
    NodePair match_next_match;
    if (graph_a_traversal[pos_a].depth == graph_b_traversal[pos_b].depth) {
        match_cost = match_cost_func(graph_a_traversal[pos_a].node_id,
                                     graph_b_traversal[pos_b].node_id);

        if (match_cost != kHugeCost) {
                match_cost += MatchInternal(graph_a_traversal, graph_b_traversal,
                                            match_cost_func,
                                            skip_cost,
                                            pos_a + 1, pos_b + 1,
                                            &match_next_match,
                                            cache);
        }
    }

    // Skip nodes from graph a.
    size_t next_pos_a = pos_a + 1;
    if (graph_a_traversal[pos_a].depth > graph_b_traversal[pos_b].depth)
        next_pos_a = graph_a_traversal[pos_a].next_depth_drop;
    NodePair skip_a_next_match;
    uint64_t skip_a_cost = kHugeCost;

    if (match_cost != 0) {
        skip_a_cost =
            ((next_pos_a - pos_a) * skip_cost) +
            MatchInternal(graph_a_traversal, graph_b_traversal,
                          match_cost_func,
                          skip_cost,
                          next_pos_a, pos_b,
                          &skip_a_next_match,
                          cache);
    }

    // Skip nodes from graph b.
    size_t next_pos_b = pos_b + 1;
    if (graph_b_traversal[pos_b].depth > graph_a_traversal[pos_a].depth)
        next_pos_b = graph_b_traversal[pos_b].next_depth_drop;
    NodePair skip_b_next_match;
    uint64_t skip_b_cost = kHugeCost;

    if (match_cost != 0) {
        skip_b_cost =
            ((next_pos_b - pos_b) * skip_cost) +
            MatchInternal(graph_a_traversal, graph_b_traversal,
                          match_cost_func,
                          skip_cost,
                          pos_a, next_pos_b,
                          &skip_b_next_match,
                          cache);
    }

    // Determine the operation with minimum cost.
    uint64_t min_cost = std::min({match_cost, skip_a_cost, skip_b_cost});

    if (min_cost == match_cost) {
        *next_match = current_pair;
        cache->insert(std::make_pair(
            current_pair, SubtreeCost(min_cost, match_next_match, true)));
    } else {
        if (min_cost == skip_a_cost) {
            *next_match = skip_a_next_match;
        } else {
            *next_match = skip_b_next_match;
        }
        cache->insert(std::make_pair(
            current_pair, SubtreeCost(min_cost, *next_match, false)));
    }

    return min_cost;
}

}    // namespace

uint64_t MatchGraphs(const Graph& graph_a,
                     const Graph& graph_b,
                     const MatchNodesCostFunc& match_cost_func,
                     uint64_t skip_cost,
                     MatchVector* matching_nodes) {
    assert(matching_nodes);

    // Generate preorder traversals of the graphs.
    NodeInfoVector graph_a_traversal;
    GeneratePreorderTraversal(graph_a, &graph_a_traversal);
    NodeInfoVector graph_b_traversal;
    GeneratePreorderTraversal(graph_b, &graph_b_traversal);

    // Match the graphs.
    MatchCache cache;
    NodePair first_match;
    uint64_t cost = MatchInternal(graph_a_traversal,
                                  graph_b_traversal,
                                  match_cost_func,
                                  skip_cost,
                                  0, 0,
                                  &first_match, &cache);

    // Output the matching nodes.
    const NodePair* current_match = &first_match;
    NodePair end_match;

    while (*current_match != end_match) {
        matching_nodes->push_back(NodePair(current_match->node_id_a(),
                                           current_match->node_id_b()));
        current_match = &cache[*current_match].next_match;
    }

    return cost;
}

}    // namespace execution
}    // namespace tibee
