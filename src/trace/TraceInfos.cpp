/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#include "trace/TraceInfos.hpp"

namespace bfs = boost::filesystem;

namespace tibee
{
namespace trace
{

TraceInfos::TraceInfos(const bfs::path& path, trace_id_t id,
                       std::unique_ptr<TraceInfos::Environment> env,
                       std::unique_ptr<TraceInfos::EventMap> eventMap) :
    _path {path},
    _id {id},
    _env {std::move(env)},
    _eventMap {std::move(eventMap)}
{
    _traceType = "unknown";

    const auto& envDomainIt = _env->find("domain");

    if (envDomainIt == _env->end()) {
        return;
    }

    const auto& domain = (*envDomainIt).second;

    // only LTTng is supported for the moment
    if (domain == "kernel" || domain == "ust") {
        _traceType = "lttng-";
        _traceType += (*envDomainIt).second;
    }
}

namespace
{

void printFieldInfos(std::ostream& out,
                     const std::unique_ptr<FieldInfos>& fieldInfos,
                     unsigned int indentLevel)
{
    if (!fieldInfos) {
        return;
    }

    std::string indent(2 * indentLevel, ' ');

    out << indent << fieldInfos->getIndex() << " " << fieldInfos->getName() << std::endl;

    if (fieldInfos->getFieldMap()) {
        for (const auto& fieldNameInfosPair : *fieldInfos->getFieldMap()) {
            printFieldInfos(out, fieldNameInfosPair.second, indentLevel + 1);
        }
    }
}

}

std::ostream& operator<<(std::ostream& out, const TraceInfos& traceInfos)
{
    // ID
    out << "ID:   " << traceInfos.getId() << std::endl;

    // path
    out << "path: " << traceInfos.getPath() << std::endl;

    // environment
    out << "environment:" << std::endl;

    for (const auto& keyValuePair : *traceInfos.getEnv()) {
        out << "  " << keyValuePair.first << " = " << keyValuePair.second << std::endl;
    }

    out << "events:" << std::endl;
    for (const auto& nameInfosPair : *traceInfos.getEventMap()) {
        const auto& eventInfos = nameInfosPair.second;

        out << "  " << nameInfosPair.first << " (" << eventInfos->getId() << ")" << std::endl;

        if (eventInfos->getFieldMap()) {
            for (const auto& fieldNameInfosPair : *eventInfos->getFieldMap()) {
                printFieldInfos(out, fieldNameInfosPair.second, 2);
            }
        }
    }

    return out;
}

}
}
