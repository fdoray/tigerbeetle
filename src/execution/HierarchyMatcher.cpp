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
#include <boost/functional/hash/hash.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "base/Constants.hpp"
#include "execution/CanonizeSequence.hpp"
#include "execution/DynamicMatching.hpp"
#include "execution/FindRepetitions.hpp"

namespace tibee {
namespace execution {

namespace
{

struct SequenceToken
{
    UIDSequence uids;
    bool is_repetition;

    bool operator==(const SequenceToken& other) const {
        return uids == other.uids && is_repetition == other.is_repetition;
    }
};

}  // namespace
}  // namespace execution
}  // namespace tibee

namespace std {

template <>
struct hash<tibee::execution::SequenceToken> {
    size_t operator()(const tibee::execution::SequenceToken& token) const {
        size_t seed = 0;
        boost::hash_combine(seed, token.uids);
        boost::hash_combine(seed, token.is_repetition);
        return seed;
    }
};

}    // namespace std

namespace tibee {
namespace execution {

namespace
{

const size_t kNumCuts = 3;

typedef std::unordered_map<SequenceToken, std::vector<size_t>> TokensMap;

typedef std::pair<size_t, size_t> Range;
typedef std::vector<Range> RangeVector;

void GenerateUIDs(
    const Node& root,
    const UniqueIdentifierFunc& uid_func,
    GraphPosition graph_position,
    UIDSequence* uids)
{
    uids->reserve(root.NumChildren());
    for (const auto& node : root)
        uids->push_back(uid_func(node, graph_position));
}

void CreateSequenceTokensMap(
    const UIDSequence& uids,
    const CanonicalSequence& canonical,
    const Range& range,
    TokensMap* tokens_map)
{
    for (size_t i = range.first; i < range.second; ++i)
    {
        SequenceToken token;
        size_t numUids = 1;
        if (canonical[i].chunk_size != 0)
            numUids = canonical[i].chunk_size;

        for (size_t j = 0; j < numUids; ++j)
            token.uids.push_back(uids[canonical[i].pos + j]);

        token.is_repetition = (canonical[i].chunk_size != 0);

        (*tokens_map)[token].push_back(i);
    }
}

void CutSequence(
    const Node& a,
    const Node& b,
    const UIDSequence& uids_a,
    const UIDSequence& uids_b,
    const CanonicalSequence& canonical_a,
    const CanonicalSequence& canonical_b,
    const Range& range_a,
    const Range& range_b,
    RangeVector* ranges_a,
    RangeVector* ranges_b,
    MatchVector* matching_nodes)
{
    // Create maps with the occurences of each token.
    TokensMap tokens_map_a;
    CreateSequenceTokensMap(uids_a, canonical_a, range_a, &tokens_map_a);
    TokensMap tokens_map_b;
    CreateSequenceTokensMap(uids_b, canonical_b, range_b, &tokens_map_b);

    // Find tokens that are repeated the same number of times in each sequence.
    std::vector<size_t> repeated_tokens_a;
    std::vector<size_t> repeated_tokens_b;

    for (const auto& token : tokens_map_a)
    {
        const auto& tokens_a = token.second;
        const auto& tokens_b = tokens_map_b[token.first];

        if (tokens_a.size() != tokens_b.size())
            continue;

        for (const auto& pos : tokens_a)
            repeated_tokens_a.push_back(pos);
        for (const auto& pos : tokens_b)
            repeated_tokens_b.push_back(pos);

        // Match the tokens.
        for (size_t i = 0; i < tokens_a.size(); ++i)
        {
            auto& canonical_token_a = canonical_a[tokens_a[i]];
            auto& canonical_token_b = canonical_b[tokens_b[i]];

            size_t num_match = 1;
            if (canonical_token_a.chunk_size != 0)
            {
                size_t num_a = canonical_token_a.chunk_size * canonical_token_a.num_repetitions;
                size_t num_b = canonical_token_b.chunk_size * canonical_token_b.num_repetitions;
                num_match = std::min(num_a, num_b);
            }

            size_t pos_a = canonical_token_a.pos;
            size_t pos_b = canonical_token_b.pos;

            for (size_t j = 0; j < num_match; ++j)
            {
                matching_nodes->push_back(NodePair(
                    a.GetChild(pos_a), b.GetChild(pos_b)));

                ++pos_a;
                ++pos_b;
            }
        }
    }

    std::sort(repeated_tokens_a.begin(), repeated_tokens_a.end());
    std::sort(repeated_tokens_b.begin(), repeated_tokens_b.end());

    // Cut the sequence at tokens.

    // - First range.
    if (repeated_tokens_a.empty())
    {
        ranges_a->push_back({range_a.first, range_a.second});
        ranges_b->push_back({range_b.first, range_b.second}); 
    }
    else if (repeated_tokens_a.front() != range_a.first ||
             repeated_tokens_b.front() != range_b.first)
    {
        ranges_a->push_back({range_a.first, repeated_tokens_a.front()});
        ranges_b->push_back({range_b.first, repeated_tokens_b.front()});
    }

    // - Other ranges.
    for (size_t i = 0; i < repeated_tokens_a.size(); ++i)
    {
        size_t begin_a = repeated_tokens_a[i] + 1;
        size_t begin_b = repeated_tokens_b[i] + 1;

        size_t end_a = range_a.second;
        size_t end_b = range_b.second;

        if (i != repeated_tokens_a.size() - 1)
        {
            end_a = repeated_tokens_a[i + 1];
            end_b = repeated_tokens_b[i + 1];
        }

        if (begin_a >= end_a && begin_b >= end_b)
            continue;

        ranges_a->push_back(Range(begin_a, end_a));
        ranges_b->push_back(Range(begin_b, end_b));
    }
}

uint64_t MatchRoots(
    const Node& a, const Node& b,
    const UniqueIdentifierFunc& uid_func,
    uint64_t skip_cost,
    MatchVector* matching_nodes)
{
    // Generate sequences of unique identifiers for the children of the roots.
    UIDSequence uids_a;
    GenerateUIDs(a, uid_func, GraphPosition::LEFT_GRAPH, &uids_a);
    UIDSequence uids_b;
    GenerateUIDs(b, uid_func, GraphPosition::RIGHT_GRAPH, &uids_b);

    // Canonize the sequences of unique identifiers.
    CanonicalSequence canonical_a;
    CanonizeSequence(uids_a, &canonical_a);
    CanonicalSequence canonical_b;
    CanonizeSequence(uids_b, &canonical_b);

    // Cut the sequences in smaller chunks.
    std::unique_ptr<RangeVector> ranges_a(new RangeVector);
    ranges_a->push_back(Range(0, canonical_a.size()));
    std::unique_ptr<RangeVector> ranges_b(new RangeVector);
    ranges_b->push_back(Range(0, canonical_b.size()));

    for (size_t i = 0; i < kNumCuts; ++i)
    {
        std::unique_ptr<RangeVector> new_ranges_a(new RangeVector);
        std::unique_ptr<RangeVector> new_ranges_b(new RangeVector);

        for (size_t j = 0; j < ranges_a->size(); ++j)
        {
            CutSequence(
                a, b, uids_a, uids_b, canonical_a, canonical_b,
                ranges_a->at(j), ranges_b->at(j),
                new_ranges_a.get(), new_ranges_b.get(),
                matching_nodes);
        }

        ranges_a = std::move(new_ranges_a);
        ranges_b = std::move(new_ranges_b);
    }

    // Apply the dynamic programming algorithm on the smaller ranges.
    uint64_t cost = 0;

    for (size_t i = 0; i < ranges_a->size(); ++i)
    {
        cost += DynamicMatching(
            canonical_a, canonical_b, uids_a, uids_b, a, b,
            ranges_a->at(i).first, ranges_a->at(i).second,
            ranges_b->at(i).first, ranges_b->at(i).second,
            skip_cost, matching_nodes);
    }

    return cost;
}

}  // namespace

uint64_t MatchGraphsHierarchical(
    const Graph& graph_a,
    const Graph& graph_b,
    const UniqueIdentifierFunc& uid_func,
    uint64_t skip_cost,
    MatchVector* matching_nodes)
{
    if (graph_a.empty())
        return graph_b.size() * skip_cost;
    if (graph_b.empty())
        return graph_a.size() * skip_cost;

    // For now, only match the first level.
    return MatchRoots(
        graph_a.GetNode(0), graph_b.GetNode(0),
        uid_func, skip_cost, matching_nodes);

    return 0;
}

}  // namespace execution
}  // namespace tibee
