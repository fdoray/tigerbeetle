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
#include "execution/DynamicMatching.hpp"

#include <algorithm>
#include <iostream>
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

// Match cache, for the dynamic programming algorithm.
typedef std::unordered_map<NodePair, SubtreeCost> MatchCache;

// Context.
struct Context
{
    Context(
        const CanonicalSequence& canonical_a,
        const CanonicalSequence& canonical_b,
        const UIDSequence& uids_a,
        const UIDSequence& uids_b,
        const Node& parent_a,
        const Node& parent_b,
        size_t end_a,
        size_t end_b,
        uint64_t skip_cost)
        : canonical_a(canonical_a), canonical_b(canonical_b),
          uids_a(uids_a), uids_b(uids_b),
          parent_a(parent_a), parent_b(parent_b),
          end_a(end_a), end_b(end_b),
          skip_cost(skip_cost)
    {}

    const CanonicalSequence& canonical_a;
    const CanonicalSequence& canonical_b;
    const UIDSequence& uids_a;
    const UIDSequence& uids_b;
    const Node& parent_a;
    const Node& parent_b;
    const size_t end_a;
    const size_t end_b;
    const uint64_t skip_cost;
    MatchCache cache;
};

// Helper functions.
NodeId NodeIdA(size_t cur, Context* context)
{
    return context->parent_a.GetChild(context->canonical_a[cur].pos);
}

NodeId NodeIdB(size_t cur, Context* context)
{
    return context->parent_b.GetChild(context->canonical_b[cur].pos);
}

const UID& UIDA(size_t cur, Context* context)
{
    return context->uids_a[context->canonical_a[cur].pos];
}

const UID& UIDB(size_t cur, Context* context)
{
    return context->uids_b[context->canonical_b[cur].pos];
}

bool CanBeMatched(size_t cur_a, size_t cur_b, Context* context)
{
    const auto& canonical_a = context->canonical_a[cur_a];
    const auto& canonical_b = context->canonical_b[cur_b];

    bool a_is_repetition = (canonical_a.chunk_size != 0);
    bool b_is_repetition = (canonical_b.chunk_size != 0);

    if (a_is_repetition != b_is_repetition)
        return false;

    if (a_is_repetition /* && b_is_repetition */)
    {
        // Repetition.
        if (canonical_a.chunk_size != canonical_b.chunk_size)
            return false;

        auto begin_a = context->uids_a.begin() + canonical_a.pos;
        auto end_a = begin_a + canonical_a.chunk_size;
        auto begin_b = context->uids_b.begin() + canonical_b.pos;

        return std::equal(begin_a, end_a, begin_b);
    }
    else
    {
        // No repetition.
        return UIDA(cur_a, context) == UIDB(cur_b, context);
    }
}

// Recursive algorithm.
uint64_t DynamicMatchingRecursive(
    size_t cur_a,
    size_t cur_b,
    size_t skip_depth,
    Context* context,
    NodePair* next_match)
{
    // Reached the end of a sequence: skip all nodes from the other sequence.
    if (cur_a >= context->end_a)
        return (context->end_b - cur_b) * context->skip_cost;
    if (cur_b >= context->end_b)
        return (context->end_a - cur_a) * context->skip_cost;

    // Look if the match is in the cache.
    NodePair cur_match(NodeIdA(cur_a, context), NodeIdB(cur_b, context));

    auto look = context->cache.find(cur_match);
    if (look != context->cache.end()) {
        if (look->second.is_matching)
            *next_match = cur_match;
        else
            *next_match = look->second.next_match;

        return look->second.cost;
    }

    // Match as much nodes as possible.
    size_t num_matched = 0;
    auto cur_a_match = cur_a;
    auto cur_b_match = cur_b;
    while (cur_a_match < context->end_a &&
           cur_b_match < context->end_b &&
           CanBeMatched(cur_a_match, cur_b_match, context)) {
        ++num_matched;
        ++cur_a_match;
        ++cur_b_match;
    }

    if (num_matched != 0)
    {
        NodePair match_after;
        uint64_t match_cost = DynamicMatchingRecursive(
            cur_a_match, cur_b_match, 0, context, &match_after);

        for (size_t i = 0; i < num_matched; ++i)
        {
            if (context->canonical_a[cur_a].chunk_size == 0)
            {
                // Simple node.
                NodePair next_match;
                if (i == (num_matched - 1))
                    next_match = match_after;
                else
                    next_match = NodePair(NodeIdA(cur_a + 1, context), NodeIdB(cur_b + 1, context));

                context->cache.insert(std::make_pair(
                    NodePair(NodeIdA(cur_a, context), NodeIdB(cur_b, context)),
                    SubtreeCost(match_cost, next_match, true)));
            }
            else
            {
                // Repetition node.
                const auto& canonical_a = context->canonical_a[cur_a];
                const auto& canonical_b = context->canonical_b[cur_b];

                auto child_index_a = canonical_a.pos;
                auto child_index_b = canonical_b.pos;

                auto seq_length_a = canonical_a.chunk_size * canonical_a.num_repetitions;
                auto seq_length_b = canonical_b.chunk_size * canonical_b.num_repetitions;

                auto seq_length = std::min(seq_length_a, seq_length_b);

                for (size_t j = 0; j < seq_length; ++j)
                {
                    NodePair next_match;
                    if (i == (num_matched - 1) && j == (seq_length - 1))
                    {
                        next_match = match_after;
                    }
                    else
                    {
                        next_match = NodePair(
                            context->parent_a.GetChild(child_index_a + 1),
                            context->parent_b.GetChild(child_index_b + 1));
                    }

                    context->cache.insert(std::make_pair(
                        NodePair(
                            context->parent_a.GetChild(child_index_a),
                            context->parent_b.GetChild(child_index_b)),
                        SubtreeCost(match_cost, next_match, true)));

                    ++child_index_a;
                    ++child_index_b;
                }
            }

            ++cur_a;
            ++cur_b;
        }

        *next_match = cur_match;

        return match_cost;
    }

    // Skip a node from graph a.
    NodePair skip_a_next_match;
    uint64_t skip_a_cost = DynamicMatchingRecursive(
        cur_a + 1, cur_b, skip_depth + 1, context, &skip_a_next_match);
    if (skip_a_cost != kHugeCost)
        skip_a_cost += context->skip_cost;

    // Skip a node from graph b.
    NodePair skip_b_next_match;
    uint64_t skip_b_cost = DynamicMatchingRecursive(
        cur_a, cur_b + 1, skip_depth + 1, context, &skip_b_next_match);
    if (skip_b_cost != kHugeCost)
        skip_b_cost += context->skip_cost;

    // Determine the operation with the minimum cost.
    uint64_t min_cost = std::min({skip_a_cost, skip_b_cost});

    if (min_cost == skip_a_cost)
        *next_match = skip_a_next_match;
    else
        *next_match = skip_b_next_match;

    context->cache.insert(std::make_pair(
        cur_match, SubtreeCost(min_cost, *next_match, false)));

    return min_cost;
}

}  // namespace

uint64_t DynamicMatching(
    const CanonicalSequence& canonical_a,
    const CanonicalSequence& canonical_b,
    const UIDSequence& uids_a,
    const UIDSequence& uids_b,
    const Node& parent_a,
    const Node& parent_b,
    size_t start_a,
    size_t end_a,
    size_t start_b,
    size_t end_b,
    uint64_t skip_cost,
    MatchVector* matching_nodes)
{
    // Initialize context.
    Context context(
        canonical_a, canonical_b,
        uids_a, uids_b,
        parent_a, parent_b,
        end_a, end_b,
        skip_cost);

    NodePair first_match;
    uint64_t cost = DynamicMatchingRecursive(start_a, start_b, 0, &context, &first_match);

    // Insert the matching nodes in the output vector.
    const NodePair* cur_match = &first_match;
    NodePair end_match;

    while (*cur_match != end_match) {
        matching_nodes->push_back(NodePair(cur_match->node_id_a(),
                                           cur_match->node_id_b()));
        cur_match = &context.cache[*cur_match].next_match;
    }

    return cost;
}

}  // namespace execution
}  // namespace tibee
