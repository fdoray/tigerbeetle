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
#ifndef _TIBEE_HISTORY_HISTORYREADER_HPP
#define _TIBEE_HISTORY_HISTORYREADER_HPP

#include <delorean/HistoryFileSource.hpp>
#include <memory>
#include <string>

#include "quark/StringQuarkDatabase.hpp"
#include "state/AttributeKey.hpp"
#include "state/AttributePath.hpp"
#include "state/AttributeTree.hpp"

namespace tibee
{
namespace history
{

/**
 * History reader.
 *
 * @author Francois Doray
 */
class HistoryReader {
public:
    HistoryReader(const std::string& historyIdentifier);

    quark::Quark IntQuark(int val);
    quark::Quark Quark(const std::string& str);
    const std::string& String(quark::Quark quark) const;

    state::AttributeKey GetAttributeKey(const state::AttributePath& path);
    state::AttributeKey GetAttributeKeyStr(const state::AttributePathStr& pathStr);
    state::AttributeKey GetAttributeKey(state::AttributeKey root, const state::AttributePath& subPath);

    void QueryHistory(state::AttributeKey attribute, delo::IntervalJar* jar);

private:
    // Quark database.
    std::unique_ptr<quark::StringQuarkDatabase> _quarks;

    // Attribute tree.
    std::unique_ptr<state::AttributeTree> _attributeTree;

    // History.
    std::unique_ptr<delo::HistoryFileSource> _history;
};

}
}

#endif // _TIBEE_HISTORY_HISTORYREADER_HPP
