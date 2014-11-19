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
#include "execution/HierarchyMatcher.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "base/Constants.hpp"
#include "execution/FindRepetitions.hpp"

namespace tibee {
namespace execution {

namespace
{

const size_t kChunkSize = 2;

using execution::Node;

// Keeps track of the nodes that have a given unique id.
typedef std::unordered_map<std::string, std::vector<size_t>> UniqueIdMap;

// Contains matching nodes.
struct MatchVectorHierarchical {
    MatchVector vector;
    std::vector<MatchVectorHierarchical*> children; 
};

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

// Context for the dynamic programming algorithm.
struct DynamicProgrammingContext
{
    Node::ChildrenIterator sub_begin_left; ///////// TO REMOVE
    Node::ChildrenIterator sub_begin_right;

    Node::ChildrenIterator sub_end_left;
    Node::ChildrenIterator sub_end_right;
    Node::ChildrenIterator begin_left;
    Node::ChildrenIterator begin_right;
    const std::vector<std::string>* uids_a;
    const std::vector<std::string>* uids_b;
    const RepetitionsMap* repetitions_a;
    const RepetitionsMap* repetitions_b;
    MatchCache cache;
};

// Hierarchical matcher.
class HierarchicalMatcher
{
public:
    HierarchicalMatcher(const Graph& graph_a,
                        const Graph& graph_b,
                        const MatchNodesCostFunc& match_cost_func,
                        const UniqueIdentifierFunc& unique_id_func,
                        uint64_t skip_cost)
        : _graph_a(graph_a), _graph_b(graph_b),
          _match_cost_func(match_cost_func),
          _unique_id_func(unique_id_func),
          _skip_cost(skip_cost)
    {
    }

    uint64_t MatchGraphs(MatchVector* match_vector)
    {
        // Apply the matching algorithm.
        MatchVectorHierarchical match_vector_hierarchical;
        match_vector_hierarchical.vector.push_back(NodePair(0, 0));
        const Node& root_a = _graph_a.GetNode(0);
        const Node& root_b = _graph_b.GetNode(0);
        uint64_t cost = MatchNodes(
            0, 0, root_a.begin(), root_a.end(), root_b.begin(), root_b.end(),
            &match_vector_hierarchical);

        // Flatten the vectors of matching nodes.
        FlattenMatchVectors(match_vector_hierarchical, match_vector);

        return cost;
    }

private:
    uint64_t MatchNodes(
        NodeId a, NodeId b,
        Node::ChildrenIterator begin_left,
        Node::ChildrenIterator end_left,
        Node::ChildrenIterator begin_right,
        Node::ChildrenIterator end_right,
        MatchVectorHierarchical* match_vector)
    {
        uint64_t cost = 0;

        const Node& node_a = _graph_a.GetNode(a);
        const Node& node_b = _graph_b.GetNode(b);

        // Match the children of the nodes.
        // - Magically match nodes that are repeated the same number of times in each graph.
        UniqueIdMap left_map;
        FillUniqueIdMap(begin_left, end_left, GraphPosition::LEFT_GRAPH, &left_map);
        UniqueIdMap right_map;
        FillUniqueIdMap(begin_right, end_right, GraphPosition::RIGHT_GRAPH, &right_map);

        std::vector<size_t> magic_matched_left;
        std::vector<size_t> magic_matched_right;
        std::vector<NodePair> magic_matched;

        for (const auto& left : left_map)
        {
            const auto& left_unique_vector = left.second;
            const auto& right_unique_vector = right_map[left.first];
            if (left_unique_vector.size() != right_unique_vector.size())
                continue;

            for (size_t i = 0; i < left_unique_vector.size(); ++i)
            {
                auto left_child = node_a.GetChild(left_unique_vector[i]);
                auto right_child = node_b.GetChild(right_unique_vector[i]);

                NodePair child_pair(left_child, right_child);
                match_vector->vector.push_back(child_pair);

                magic_matched_left.push_back(left_unique_vector[i]);
                magic_matched_right.push_back(right_unique_vector[i]);
                magic_matched.push_back(NodePair(left_unique_vector[i], right_unique_vector[i]));

                // TODO: Match the children....
            }
        }

        std::sort(magic_matched_left.begin(), magic_matched_left.end());
        std::sort(magic_matched_right.begin(), magic_matched_right.end());

        // - Find repetitions.
        std::vector<std::string> uids_a;
        for (auto cur_a = begin_left; cur_a != end_left; ++cur_a)
            uids_a.push_back(_unique_id_func(*cur_a, GraphPosition::LEFT_GRAPH));
        RepetitionsMap repetitions_a;
        FindRepetitionsMap(uids_a, kChunkSize, &repetitions_a);

        std::vector<std::string> uids_b;
        for (auto cur_b = begin_right; cur_b != end_right; ++cur_b)
            uids_b.push_back(_unique_id_func(*cur_b, GraphPosition::RIGHT_GRAPH));
        RepetitionsMap repetitions_b;
        FindRepetitionsMap(uids_b, kChunkSize, &repetitions_b);

        // - Match the nodes before the first magic.
        auto first_sub_begin_left = begin_left;
        auto first_sub_begin_right = begin_right;
        auto first_sub_end_left = end_left;
        auto first_sub_end_right = end_right;

        if (!magic_matched.empty()) {
            first_sub_end_left = begin_left + magic_matched_left.front();
            first_sub_end_right = begin_right + magic_matched_right.front();
        }

        cost += MatchSubsequences(
            first_sub_begin_left, first_sub_end_left,
            first_sub_begin_right, first_sub_end_right,
            begin_left, begin_right,
            uids_a, uids_b,
            repetitions_a, repetitions_b,
            match_vector);

        // - Match the remaining nodes (gaps between magic).
        for (const NodePair& child_pair : magic_matched)
        {
            // Find the offsets of the next magically matched nodes.
            auto next_left = std::upper_bound(magic_matched_left.begin(), magic_matched_left.end(), child_pair.node_id_a());
            auto next_right = std::upper_bound(magic_matched_right.begin(), magic_matched_right.end(), child_pair.node_id_b());

            auto sub_begin_left = begin_left + child_pair.node_id_a() + 1;
            auto sub_end_left = end_left;
            if (next_left != magic_matched_left.end())
                sub_end_left = begin_left + *next_left;

            auto sub_begin_right = begin_right + child_pair.node_id_b() + 1;
            auto sub_end_right = end_right;
            if (next_right != magic_matched_right.end())
                sub_end_right = begin_right + *next_right;

            // Match the corresponding gaps.
            cost += MatchSubsequences(
                sub_begin_left, sub_end_left,
                sub_begin_right, sub_end_right,
                begin_left, begin_right,
                uids_a, uids_b,
                repetitions_a, repetitions_b,
                match_vector);
        }

        return cost;
    }

    uint64_t MatchSubsequences(Node::ChildrenIterator sub_begin_left,
                               Node::ChildrenIterator sub_end_left,
                               Node::ChildrenIterator sub_begin_right,
                               Node::ChildrenIterator sub_end_right,
                               Node::ChildrenIterator begin_left,
                               Node::ChildrenIterator begin_right,
                               const std::vector<std::string>& uids_a,
                               const std::vector<std::string>& uids_b,
                               const RepetitionsMap& repetitions_a,
                               const RepetitionsMap& repetitions_b,
                               MatchVectorHierarchical* match_vector)
    {
        // Create a context for the dynamic programming algorithm.
        DynamicProgrammingContext context;
        context.sub_begin_left = sub_begin_left;   // A SUPPRIMER
        context.sub_begin_right = sub_begin_right;

        context.sub_end_left = sub_end_left;
        context.sub_end_right = sub_end_right;
        context.begin_left = begin_left;
        context.begin_right = begin_right;
        context.uids_a = &uids_a;
        context.uids_b = &uids_b;
        context.repetitions_a = &repetitions_a;
        context.repetitions_b = &repetitions_b;

        // Apply the dynamic programming algorithm.
        NodePair first_match;
        uint64_t cost = MatchSubsequencesRecursive(
            sub_begin_left, sub_begin_right,
            &context, 0, &first_match);

        // Retrieve the matching nodes.
        const NodePair* current_match = &first_match;
        NodePair end_match;

        while (*current_match != end_match) {
            match_vector->vector.push_back(
                NodePair(current_match->node_id_a(), current_match->node_id_b()));
            current_match = &context.cache[*current_match].next_match;

            // TODO: Also retrieve the children match vectors...
        }

        return cost;
    }

    uint64_t MatchSubsequencesRecursive(Node::ChildrenIterator cur_left,
                                        Node::ChildrenIterator cur_right,
                                        DynamicProgrammingContext* context,
                                        size_t num_skips,
                                        NodePair* next_match)
    {
        if (num_skips > 25) 
            return kHugeCost;

        // Reached the end of a sequence: skip all nodes from the other sequence.
        if (cur_left >= context->sub_end_left)
            return std::distance(cur_right, context->sub_end_right) * _skip_cost;
        else if (cur_right >= context->sub_end_right)
            return std::distance(cur_left, context->sub_end_left) * _skip_cost;

        // Look if the match is in the cache.
        NodePair current_pair(*cur_left, *cur_right);

        auto look = context->cache.find(current_pair);
        if (look != context->cache.end()) {
            if (look->second.is_matching)
                *next_match = current_pair;
            else
                *next_match = look->second.next_match;

            return look->second.cost;
        }

        // Match 2 sequences of repetitions.
        size_t offset_a = static_cast<size_t>(std::distance(context->begin_left, cur_left));
        size_t offset_b = static_cast<size_t>(std::distance(context->begin_right, cur_right));
        auto repetitions_a_it = context->repetitions_a->find(offset_a);
        auto repetitions_b_it = context->repetitions_b->find(offset_b);
        bool a_is_repetition = repetitions_a_it != context->repetitions_a->end();
        bool b_is_repetition = repetitions_b_it != context->repetitions_b->end();

        if (a_is_repetition && b_is_repetition &&
            std::equal(context->uids_a->begin() + offset_a,
                       context->uids_a->begin() + offset_a + kChunkSize,
                       context->uids_b->begin() + offset_b))
        {
            // Found repetitions of the same chunk.
            // TODO: For now, matching the 2 subsequences is free.

            // Find the cost of matching the sequences that follow the repetitions.
            NodePair next_match_repetition;
            uint64_t cost_repetition = MatchSubsequencesRecursive(
                cur_left + (repetitions_a_it->second * kChunkSize),
                cur_right + (repetitions_b_it->second * kChunkSize),
                context, 0,
                &next_match_repetition);

            // Match the repeated nodes.
            size_t shortest_repetition = std::min(repetitions_a_it->second, repetitions_b_it->second);
            for (size_t i = 0; i < shortest_repetition * kChunkSize; ++i)
            {

                NodePair next_match_inside_repetition;
                if (i == (shortest_repetition * kChunkSize - 1))
                    next_match_inside_repetition = next_match_repetition;
                else
                    next_match_inside_repetition = NodePair(*(cur_left + 1), *(cur_right + 1));

                context->cache.insert(std::make_pair(
                    NodePair(*cur_left, *cur_right),
                    SubtreeCost(cost_repetition,
                                next_match_inside_repetition,
                                true)));

                ++cur_left;
                ++cur_right;

                // TODO: Match children...
            }

            *next_match = current_pair;

            return cost_repetition;
        }

        // Match as much identical nodes as possible.
        // Empty uids cannot be matched by themselves. They must be matched with their previous node.
        uint64_t match_cost = _match_cost_func(current_pair.node_id_a(), current_pair.node_id_b());
        if (a_is_repetition || b_is_repetition)
            match_cost = kHugeCost;

        if (match_cost == 0 && !context->uids_a->at(offset_a).empty())
        {
            *next_match = current_pair;

            auto tmp_cur_left = cur_left + 1;
            auto tmp_cur_right = cur_right + 1;

            // Determine how much more nodes we can match.
            size_t num_extra_match = 0;
            while (tmp_cur_left < context->sub_end_left &&
                   tmp_cur_right < context->sub_end_right &&
                   _match_cost_func(*tmp_cur_left, *tmp_cur_right) == 0 &&
                   context->cache.find(NodePair(*tmp_cur_left, *tmp_cur_right)) == context->cache.end() &&
                   context->repetitions_a->find(std::distance(context->begin_left, tmp_cur_left)) == context->repetitions_a->end() &&
                   context->repetitions_b->find(std::distance(context->begin_right, tmp_cur_right)) == context->repetitions_b->end())
            {
                ++num_extra_match;
                ++tmp_cur_left;
                ++tmp_cur_right;
            }

            // Match the nodes that follow the sequence of equal nodes.
            auto cur_after_eq_left = cur_left + num_extra_match + 1;
            auto cur_after_eq_right = cur_right + num_extra_match + 1;

            NodePair after_match_pair;

            uint64_t after_match_cost = MatchSubsequencesRecursive(
                cur_after_eq_left, cur_after_eq_right,
                context, 0, &after_match_pair);

            for (size_t i = 0; i < num_extra_match + 1; ++i) {
                NodePair cacheNextPair;
                if (i == num_extra_match)
                    cacheNextPair = after_match_pair;
                else
                    cacheNextPair = NodePair(*(cur_left + 1), *(cur_right + 1));

                context->cache.insert(std::make_pair(
                    NodePair(*cur_left, *cur_right),
                    SubtreeCost(after_match_cost, cacheNextPair, true)));

                ++cur_left;
                ++cur_right;
            }

            return after_match_cost;
        }

        NodePair match_next_match;
        if (match_cost != kHugeCost)
        {
            size_t new_num_skips = 0;
            if (context->uids_a->at(offset_a).empty())
                new_num_skips = num_skips;

            match_cost += MatchSubsequencesRecursive(
                cur_left + 1, cur_right + 1, context, new_num_skips, &match_next_match);
        }

        // Skip a node from graph a.
        NodePair skip_a_next_match;
        uint64_t skip_a_cost = MatchSubsequencesRecursive(
            cur_left + 1, cur_right,
            context, num_skips + 1, &skip_a_next_match);
        if (skip_a_cost != kHugeCost)
            skip_a_cost += _skip_cost;

        // Skip a node from graph b.
        NodePair skip_b_next_match;
        uint64_t skip_b_cost = MatchSubsequencesRecursive(
            cur_left, cur_right + 1,
            context, num_skips + 1, &skip_b_next_match);
        if (skip_b_cost != kHugeCost)
            skip_b_cost += _skip_cost;

        // Determine the operation with minimum cost.
        uint64_t min_cost = std::min({match_cost, skip_a_cost, skip_b_cost});

        if (min_cost == match_cost)
        {
            *next_match = current_pair;
            context->cache.insert(std::make_pair(
                current_pair, SubtreeCost(min_cost, match_next_match, true)));
        }
        else
        {
            if (min_cost == skip_a_cost)
                *next_match = skip_a_next_match;
            else
                *next_match = skip_b_next_match;

            context->cache.insert(std::make_pair(
                current_pair, SubtreeCost(min_cost, *next_match, false)));
        }

        return min_cost;
    }

    void FillUniqueIdMap(Node::ChildrenIterator begin,
                         Node::ChildrenIterator end,
                         GraphPosition position,
                         UniqueIdMap* map)
    {
        auto cur = begin;
        for (; cur != end; ++cur) {
            auto unique_id = _unique_id_func(*cur, position);
            if (unique_id.empty())
                continue;
            (*map)[unique_id].push_back(
                static_cast<size_t>(std::distance(begin, cur)));
        }
    }

    void FlattenMatchVectors(const MatchVectorHierarchical& in, MatchVector* out)
    {
        std::copy(in.vector.begin(), in.vector.end(), std::back_inserter(*out));
        for (const auto& child : in.children)
            FlattenMatchVectors(*child, out);
    }

    const Graph& _graph_a;
    const Graph& _graph_b;
    const MatchNodesCostFunc& _match_cost_func;
    const UniqueIdentifierFunc& _unique_id_func;
    uint64_t _skip_cost;
};

}  // namespace

uint64_t MatchGraphsHierarchical(
    const Graph& graph_a,
    const Graph& graph_b,
    const MatchNodesCostFunc& match_cost_func,
    const UniqueIdentifierFunc& unique_id_func,
    uint64_t skip_cost,
    MatchVector* matching_nodes)
{
    if (graph_a.empty())
        return graph_b.size() * skip_cost;
    if (graph_b.empty())
        return graph_a.size() * skip_cost;

    HierarchicalMatcher matcher(graph_a, graph_b, match_cost_func, unique_id_func, skip_cost);
    return matcher.MatchGraphs(matching_nodes);
}

}  // namespace execution
}  // namespace tibee
