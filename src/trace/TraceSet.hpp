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
#ifndef _TIBEE_TRACE_TRACESET_HPP
#define _TIBEE_TRACE_TRACESET_HPP

#include <memory>
#include <cstdint>
#include <set>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/utility.hpp>
#include <babeltrace/babeltrace.h>

#include "base/BasicTypes.hpp"
#include "trace/babeltrace-internals.h"
#include "trace/TraceSetIterator.hpp"
#include "trace/TraceInfos.hpp"

namespace tibee {
namespace trace {

/**
 * A trace set.
 *
 * All sorts of traces may be added to this set and then events may be
 * read in a trace format-agnostic way.
 *
 * Trace formats are automagically recognized, either using file
 * extensions or by inspecting the actual data or directory structure.
 *
 * @author Philippe Proulx
 */
class TraceSet :
    boost::noncopyable
{
public:
    typedef TraceSetIterator Iterator;

public:
    /**
     * Builds an empty trace set.
     */
    TraceSet();

    virtual ~TraceSet();

    /**
     * Adds trace with path \p path to the trace set.
     *
     * @param path Path of trace to add
     * @returns    True if the trace was successfully added
     */
    bool addTrace(const boost::filesystem::path& path);

    /**
     * Returns the begin timestamp of the set.
     *
     * @returns Begin timestamp of the set
     */
    timestamp_t getBegin() const;

    /**
     * Returns the end timestamp of the set.
     *
     * @returns End timestamp of the set
     */
    timestamp_t getEnd() const;

    /**
     * Returns an iterator pointing to the first event of the set.
     *
     * @returns Iterator pointing to the first event of the set
     */
    Iterator begin() const;

    /**
     * Returns an iterator pointing after the last event of the set.
     *
     * @returns Iterator pointing after the last event of the set
     */
    Iterator end() const;

    /**
     * Returns the set of trace informations.
     *
     * @returns Trace infos set
     */
    const std::set<std::unique_ptr<TraceInfos>>& getTracesInfos() const
    {
        return _tracesInfos;
    }

private:
    void seekBegin() const;
    static std::unique_ptr<TraceInfos::EventMap> getEventMap(::bt_ctf_event_decl* const* eventDeclList,
                                                             unsigned int count);
    static std::unique_ptr<EventInfos> getEventInfos(const ::tibee_bt_ctf_event_decl* tibeeBtCtfEventDecl,
                                                     const std::string& eventName);
    static std::unique_ptr<FieldInfos> getFieldInfos(const ::tibee_bt_declaration* tibeeBtDecl,
                                                     std::string name,
                                                     field_index_t index);
    bool addTraceToSet(const boost::filesystem::path& path, int traceHandle);

private:
    std::set<std::unique_ptr<TraceInfos>> _tracesInfos;
    ::bt_context* _btCtx;
    ::bt_iter* _btIter;
    ::bt_ctf_iter* _btCtfIter;
};

}  // namespace trace
}  // namespace tibee

#endif  // _TIBEE_TRACE_TRACESET_HPP
