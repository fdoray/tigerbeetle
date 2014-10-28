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
#include "history/HistoryReader.hpp"

#include <boost/filesystem.hpp>

#include "base/Constants.hpp"
#include "history/AttributeTreeReader.hpp"
#include "history/QuarkDatabaseReader.hpp"

namespace tibee
{
namespace history
{

HistoryReader::HistoryReader(const std::string& historyIdentifier)
{
    namespace bfs = boost::filesystem;
    bfs::path historyDir(kStateHistoryDirectory);

    // Read the quark database.
    _quarks.reset(new quark::StringQuarkDatabase);
    bfs::path quarkPath = historyDir / (historyIdentifier + kQuarkDatabaseFilename);
    ReadQuarkDatabase(quarkPath, _quarks.get());

    // Read the attribute tree.
    _attributeTree.reset(new state::AttributeTree);
    bfs::path attributePath = historyDir / (historyIdentifier + kAttributeTreeFilename);
    ReadAttributeTree(attributePath, _attributeTree.get());

    // Open the history tree.
    _history.reset(new delo::HistoryFileSource);
    bfs::path historyPath = historyDir / (historyIdentifier + kHistoryTreeFilename);
    _history->open(historyPath.string());
}

quark::Quark HistoryReader::IntQuark(int val)
{
    return _quarks->IntQuark(val);
}

quark::Quark HistoryReader::Quark(const std::string& str)
{
    return _quarks->StrQuark(str);
}

const std::string& HistoryReader::String(quark::Quark quark) const
{
    return _quarks->String(quark);
}

state::AttributeKey HistoryReader::GetAttributeKey(const state::AttributePath& path)
{
    return _attributeTree->CreateNodeKey(path);
}

state::AttributeKey HistoryReader::GetAttributeKeyStr(const state::AttributePathStr& pathStr)
{
    state::AttributePath path;
    path.reserve(pathStr.size());
    for (const auto& str : pathStr)
        path.push_back(Quark(str));
    return GetAttributeKey(path);
}

state::AttributeKey HistoryReader::GetAttributeKey(state::AttributeKey root, const state::AttributePath& subPath)
{
    return _attributeTree->CreateNodeKey(root, subPath);
}

void HistoryReader::QueryHistory(state::AttributeKey attribute, delo::IntervalJar* jar)
{
    
}

}
}
