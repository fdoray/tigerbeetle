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
#include <babeltrace/ctf/iterator.h>

#include "trace/TraceSetIterator.hpp"
#include "trace/TraceSet.hpp"
#include "trace/TraceUtils.hpp"
#include "trace/babeltrace-internals.h"
#include "trace/ex/TraceSet.hpp"

namespace bfs = boost::filesystem;

namespace tibee
{
namespace trace
{

TraceSet::TraceSet()
{
    _btCtx = ::bt_context_create();

    if (!_btCtx) {
        throw ex::TraceSet {"cannot create Babeltrace context"};
    }

    ::bt_iter_pos beginPos;
    beginPos.type = ::BT_SEEK_BEGIN;
    beginPos.u.seek_time = 0;

    _btCtfIter = ::bt_ctf_iter_create(_btCtx, &beginPos, nullptr);

    if (!_btCtfIter) {
        ::bt_context_put(_btCtx);

        throw ex::TraceSet {"cannot create Babeltrace iterator"};
    }

    _btIter = ::bt_ctf_get_iter(_btCtfIter);
}

TraceSet::~TraceSet()
{
    ::bt_ctf_iter_destroy(_btCtfIter);
    ::bt_context_put(_btCtx);
}

void TraceSet::seekBegin() const
{
    ::bt_iter_pos beginPos;
    beginPos.type = ::BT_SEEK_BEGIN;
    beginPos.u.seek_time = 0;

    ::bt_iter_set_pos(_btIter, &beginPos);
}

std::unique_ptr<FieldInfos> TraceSet::getFieldInfos(const ::tibee_bt_declaration* tibeeBtDecl,
                                                    std::string name,
                                                    field_index_t index)
{
    if (!tibeeBtDecl) {
        return nullptr;
    }

    // do not pick the first '_' character in the name
    if (name.at(0) == '_') {
        name = name.substr(1);
    }

    if (tibeeBtDecl->id != ::CTF_TYPE_STRUCT) {
        // no field map
        return std::unique_ptr<FieldInfos> {
            new FieldInfos {
                index, name, nullptr
            }
        };
    }

    auto tibeeDeclStruct = reinterpret_cast<const ::tibee_declaration_struct*>(tibeeBtDecl);

    std::unique_ptr<FieldInfos::FieldMap> fieldMap {new FieldInfos::FieldMap};

    if (!tibeeDeclStruct->fields) {
        // structure has uninitialized "fields" member (weird)
        return std::unique_ptr<FieldInfos> {
            new FieldInfos {
                index, name, nullptr
            }
        };
    }

    // iterate structure fields
    auto tibeeDeclStructFields = tibeeDeclStruct->fields;

    for (std::size_t x = 0; x < tibeeDeclStructFields->len; ++x) {
        // get declaration field from array
        auto tibeeDeclField = &g_array_index(tibeeDeclStructFields,
                                             ::tibee_declaration_field,
                                             x);

        // get declaration
        auto fieldTibeeBtDecl = tibeeDeclField->declaration;

        // get name
        auto fieldName = static_cast<const char*>(::g_quark_to_string(tibeeDeclField->name));

        // ready for recursion!
        auto fieldInfos = TraceSet::getFieldInfos(fieldTibeeBtDecl,
                                                  fieldName,
                                                  x);

        // set it now
        (*fieldMap)[fieldName] = std::move(fieldInfos);
    }

    return std::unique_ptr<FieldInfos> {
        new FieldInfos {
            index, name, std::move(fieldMap)
        }
    };
}

std::unique_ptr<EventInfos> TraceSet::getEventInfos(const ::tibee_bt_ctf_event_decl* tibeeBtCtfEventDecl,
                                                    const std::string& eventName)
{
    // get tigerbeetle event ID
    auto ctfEventId = tibeeBtCtfEventDecl->parent.id;
    auto ctfStreamId = tibeeBtCtfEventDecl->parent.stream_id;

    auto eventId = TraceUtils::tibeeEventIdFromCtf(ctfStreamId, ctfEventId);

    // create field map
    std::unique_ptr<FieldInfos::FieldMap> fieldMap {new FieldInfos::FieldMap};

    // get "fields" field infos
    std::unique_ptr<FieldInfos> fieldsFieldInfos;

    if (tibeeBtCtfEventDecl->parent.fields_decl) {
        auto fieldsTibeeBtDecl = reinterpret_cast<const ::tibee_bt_declaration*>(tibeeBtCtfEventDecl->parent.fields_decl);

        fieldsFieldInfos = TraceSet::getFieldInfos(fieldsTibeeBtDecl, "fields", 0);
    }

    (*fieldMap)["fields"] = std::move(fieldsFieldInfos);

    // get "context" field infos
    std::unique_ptr<FieldInfos> contextFieldInfos;

    if (tibeeBtCtfEventDecl->parent.context_decl) {
        auto contextTibeeBtDecl = reinterpret_cast<const ::tibee_bt_declaration*>(tibeeBtCtfEventDecl->parent.context_decl);

        contextFieldInfos = TraceSet::getFieldInfos(contextTibeeBtDecl, "context", 1);
    }

    (*fieldMap)["context"] = std::move(contextFieldInfos);

    // create event infos
    std::unique_ptr<EventInfos> eventInfos {
        new EventInfos {
            eventId, eventName, std::move(fieldMap)
        }
    };

    return eventInfos;
}

std::unique_ptr<TraceInfos::EventMap> TraceSet::getEventMap(::bt_ctf_event_decl* const* eventDeclList,
                                                            unsigned int count)
{
    std::unique_ptr<TraceInfos::EventMap> eventMap {
        new TraceInfos::EventMap
    };

    // map event names to event infos
    for (std::size_t x = 0; x < count; ++x) {
        auto eventDecl = eventDeclList[x];
        auto eventName = ::bt_ctf_get_decl_event_name(eventDecl);
        auto tibeeCtfEventDecl = reinterpret_cast<const ::tibee_bt_ctf_event_decl*>(eventDecl);

        // get event infos
        auto eventInfos = TraceSet::getEventInfos(tibeeCtfEventDecl, eventName);

        // put association into map
        (*eventMap)[eventName] = std::move(eventInfos);
    }

    return eventMap;
}

bool TraceSet::addTraceToSet(const bfs::path& path, int traceHandle)
{
    // get list of event declarations for this trace handle
    ::bt_ctf_event_decl* const* eventDeclList;
    unsigned int count;

    auto ret = ::bt_ctf_get_event_decl_list(traceHandle, _btCtx,
                                            &eventDeclList, &count);

    if (ret < 0) {
        // internal libbabeltrace error
        return false;
    }

    if (count == 0) {
        // unusual
        return false;
    }

    // create trace environment
    std::unique_ptr<TraceInfos::Environment> env {
        new TraceInfos::Environment
    };

    // use first event declaration to retrieve the environment
    auto firstEventDecl = eventDeclList[0];
    auto tibeeEventDecl = reinterpret_cast<const ::tibee_bt_ctf_event_decl*>(firstEventDecl);
    auto tibeeTraceEnv = tibeeEventDecl->parent.stream->trace->env;

    // fill environment map now
    if (std::strlen(tibeeTraceEnv.procname) > 0) {
        (*env)["procname"] = tibeeTraceEnv.procname;
    }

    if (std::strlen(tibeeTraceEnv.hostname) > 0) {
        (*env)["hostname"] = tibeeTraceEnv.hostname;
    }

    if (std::strlen(tibeeTraceEnv.domain) > 0) {
        (*env)["domain"] = tibeeTraceEnv.domain;
    }

    if (std::strlen(tibeeTraceEnv.sysname) > 0) {
        (*env)["sysname"] = tibeeTraceEnv.sysname;
    }

    if (std::strlen(tibeeTraceEnv.release) > 0) {
        (*env)["release"] = tibeeTraceEnv.release;
    }

    if (std::strlen(tibeeTraceEnv.version) > 0) {
        (*env)["version"] = tibeeTraceEnv.version;
    }

    if (tibeeTraceEnv.vpid >= 0) {
        (*env)["vpid"] = std::to_string(tibeeTraceEnv.vpid);
    }

    // get event map
    auto eventMap = TraceSet::getEventMap(eventDeclList, count);

    // create trace infos
    std::unique_ptr<TraceInfos> traceInfos {
        new TraceInfos {
            path,
            static_cast<trace_id_t>(traceHandle),
            std::move(env),
            std::move(eventMap)
        }
    };

    // add to our set of trace infos
    _tracesInfos.insert(std::move(traceInfos));

    return true;
}

bool TraceSet::addTrace(const boost::filesystem::path& path)
{
    for (const auto& traceInfo : _tracesInfos) {
        if (traceInfo->getPath() == path) {
            // already added
            return false;
        }
    }

    // TODO: eventually, here would be the place to detect the trace type

    auto ret = ::bt_context_add_trace(_btCtx, path.string().c_str(), "ctf",
                                      nullptr, nullptr, nullptr);

    if (ret < 0) {
        // Babeltrace error
        return false;
    }

    // add to our set now
    return this->addTraceToSet(path, ret);
}

timestamp_t TraceSet::getBegin() const
{
    // ignore if no trace is loaded
    if (_tracesInfos.empty()) {
        return -1;
    }

    // save position (iterator might be shared)
    auto savedPos = ::bt_iter_get_pos(_btIter);

    // go to beginning
    this->seekBegin();

    // read event
    auto event = bt_ctf_iter_read_event(_btCtfIter);

    if (!event) {
        ::bt_iter_set_pos(_btIter, savedPos);
        ::bt_iter_free_pos(savedPos);
        return -1;
    }

    // read event timestamp
    auto ts = ::bt_ctf_get_timestamp(event);

    // restore saved position
    ::bt_iter_set_pos(_btIter, savedPos);
    ::bt_iter_free_pos(savedPos);

    return static_cast<timestamp_t>(ts);
}

timestamp_t TraceSet::getEnd() const
{
    // ignore if no trace is loaded
    if (_tracesInfos.empty()) {
        return -1;
    }

    // save position (iterator might be shared)
    auto savedPos = ::bt_iter_get_pos(_btIter);

    // go to end
    ::bt_iter_pos endPos;
    endPos.type = ::BT_SEEK_LAST;
    endPos.u.seek_time = 0;
    ::bt_iter_set_pos(_btIter, &endPos);

    // read event
    auto event = bt_ctf_iter_read_event(_btCtfIter);

    if (!event) {
        ::bt_iter_set_pos(_btIter, savedPos);
        ::bt_iter_free_pos(savedPos);
        return -1;
    }

    // read event timestamp
    auto ts = ::bt_ctf_get_timestamp(event);

    // restore saved position
    ::bt_iter_set_pos(_btIter, savedPos);
    ::bt_iter_free_pos(savedPos);

    return static_cast<timestamp_t>(ts);
}


TraceSet::Iterator TraceSet::begin() const
{
    // go back to beginning (will also affect all existing iterators)
    this->seekBegin();

    // create new iterator
    return TraceSet::Iterator {_btCtfIter};
}


TraceSet::Iterator TraceSet::end() const
{
    // "end" is just a null iterator
    return TraceSet::Iterator {nullptr};
}

}
}
