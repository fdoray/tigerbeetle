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
#include <string>
#include <cstring>

#include "trace/babeltrace-internals.h"
#include "trace/EventValueFactory.hpp"
#include "trace/TraceUtils.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee
{
namespace trace
{

const char EventValue::kNameField[] = "name";
const char EventValue::kTimestampField[] = "ts";
const char EventValue::kFieldsField[] = "fields";
const char EventValue::kContextField[]= "context";
const char EventValue::kStreamPacketContextField[]= "stream-packet-context";

const size_t EventValue::kNameFieldOffset = 0;
const size_t EventValue::kTimestampFieldOffset = 1;
const size_t EventValue::kFieldsFieldOffset = 2;
const size_t EventValue::kContextFieldOffset = 3;
const size_t EventValue::kStreamPacketContextFieldOffset = 4;
const size_t EventValue::kNumFields = 5;

namespace {
const char* kEventTopLevelFields[] = {
    EventValue::kNameField,
    EventValue::kTimestampField,
    EventValue::kFieldsField,
    EventValue::kContextField,
    EventValue::kStreamPacketContextField,
};
}  // namespace

EventValue::EventValue(const EventValueFactory* valueFactory) :
    _valueFactory {valueFactory}
{
}

size_t EventValue::Length() const
{
    return kNumFields;
}

bool EventValue::HasField(const std::string& name) const
{
    if (std::strcmp(kNameField, name.c_str()) == 0 ||
        std::strcmp(kTimestampField, name.c_str()) == 0) {
        return true;
    }
        
    if (std::strcmp(kFieldsField, name.c_str()) == 0)
        return getFields() != nullptr;

    if (std::strcmp(kContextField, name.c_str()) == 0)
        return getContext() != nullptr;

    if (std::strcmp(kStreamPacketContextField, name.c_str()) == 0)
        return getStreamPacketContext() != nullptr;

    return false;
}

const value::Value* EventValue::GetField(const std::string& name) const
{
    for (size_t i = 0; i < kNumFields; ++i)
    {
        if (std::strcmp(kEventTopLevelFields[i], name.c_str()) == 0)
            return at(i);
    }

    return nullptr;
}

const value::Value* EventValue::at(size_t index) const
{
    if (index == kNameFieldOffset) {
        _name.SetValue(getName());
        return &_name;
    }

    if (index == kTimestampFieldOffset) {
        _ts.SetValue(getTimestamp());
        return &_ts;
    }
        
    if (index == kFieldsFieldOffset)
        return getFields();

    if (index == kContextFieldOffset)
        return getContext();

    if (index == kStreamPacketContextFieldOffset)
        return getStreamPacketContext();

    return nullptr;
}

value::StructValueBase::Iterator EventValue::fields_begin() const
{
    return value::StructValueBase::Iterator(new IteratorImpl(this, 0));
}

value::StructValueBase::Iterator EventValue::fields_end() const
{
    return value::StructValueBase::Iterator(new IteratorImpl(this, kNumFields));
}

const char* EventValue::getName() const
{
    return ::bt_ctf_event_name(_btEvent);
}

std::string EventValue::getNameStr() const
{
    return std::string {this->getName()};
}

trace_cycles_t EventValue::getCycles() const
{
    return static_cast<trace_cycles_t>(::bt_ctf_get_cycles(_btEvent));
}

timestamp_t EventValue::getTimestamp() const
{
    return static_cast<timestamp_t>(::bt_ctf_get_timestamp(_btEvent));
}

const value::Value* EventValue::getTopLevelScope(::bt_ctf_scope topLevelScope) const
{
    // get fields scope
    auto scopeDef = ::bt_ctf_get_top_level_scope(_btEvent, topLevelScope);

    // make sure it's a struct
    if (scopeDef && ::bt_ctf_field_type(::bt_ctf_get_decl_from_def(scopeDef)) == ::CTF_TYPE_STRUCT) {
        /* We know for sure a DictEventValue will be returned here because
         * of the check above.
         */
        return _valueFactory->buildEventValue(scopeDef, _btEvent);
    }

    return std::addressof(_emptyStruct);
}

const value::Value* EventValue::getFields() const
{
    if (!_fieldsDict) {
        _fieldsDict = this->getTopLevelScope(::BT_EVENT_FIELDS);
    }

    return _fieldsDict;
}

const value::Value* EventValue::getContext() const
{
    if (!_contextDict) {
        _contextDict = this->getTopLevelScope(::BT_EVENT_CONTEXT);
    }

    return _contextDict;
}

const value::Value* EventValue::getStreamEventContext() const
{
    if (!_streamEventContextDict) {
        _streamEventContextDict = this->getTopLevelScope(::BT_STREAM_EVENT_CONTEXT);
    }

    return _streamEventContextDict;
}

const value::Value* EventValue::getStreamPacketContext() const
{
    if (!_streamPacketContextDict) {
        _streamPacketContextDict = this->getTopLevelScope(::BT_STREAM_PACKET_CONTEXT);
    }

    return _streamPacketContextDict;
}

void EventValue::setPrivateEvent(::bt_ctf_event* btEvent)
{
    // set the attribute
    _btEvent = btEvent;

    // reset cached pointers
    _fieldsDict = nullptr;
    _contextDict = nullptr;
    _streamEventContextDict = nullptr;
    _streamPacketContextDict = nullptr;

    /* In CTF, an event ID is unique within its _stream_, so in order
     * to keep a real unique ID for the whole trace, we include the
     * CTF stream ID and the CTF event ID in our version of an event ID.
     * The Event class user should not have to care about internal
     * concepts like streams, specific to a trace format.
     *
     * Left-shifting the stream ID by 20 positions makes it possible
     * to have 1 mibievents per stream and 4096 different streams per
     * trace, which seems reasonable.
     */
    auto tibeeBtCtfEvent = reinterpret_cast<::tibee_bt_ctf_event*>(btEvent);
    auto tibeeStream = tibeeBtCtfEvent->parent->stream;
    auto ctfEventId = tibeeStream->event_id;
    auto ctfStreamId = tibeeStream->stream_id;
    _id = TraceUtils::tibeeEventIdFromCtf(ctfStreamId, ctfEventId);

    /* Let's use the trace handle (an integer starting at 0) here, which
     * is unique for each trace in the same Babeltrace context (and we
     * only have one).
     */
    _traceId = tibeeStream->stream_class->trace->parent.handle->id;
}

EventValue::IteratorImpl::IteratorImpl(
    const EventValue* eventValue, size_t index)
    : _eventValue {eventValue},
      _currentIndex(index)
{
}

value::StructValueBase::IteratorImpl&
    EventValue::IteratorImpl::operator++()
{
    ++_currentIndex;
    _currentPair.reset(nullptr);
    return *this;
}

bool EventValue::IteratorImpl::operator==(
    const StructValueBase::IteratorImpl& other) const
{
    auto other_cast = reinterpret_cast<const EventValue::IteratorImpl*>(
        std::addressof(other));
    return _currentIndex == other_cast->_currentIndex;
}

bool EventValue::IteratorImpl::operator!=(
    const StructValueBase::IteratorImpl& other) const {
    auto other_cast = reinterpret_cast<const EventValue::IteratorImpl*>(
        std::addressof(other));
    return _currentIndex != other_cast->_currentIndex;
}

const std::pair<const std::string, const value::Value*>&
    EventValue::IteratorImpl::operator*() const {
    if (_currentPair.get() == nullptr) {
        _currentPair.reset(
            new std::pair<const std::string, const value::Value*> {
                kEventTopLevelFields[_currentIndex],
                _eventValue->at(_currentIndex)
            });
    }
    return *_currentPair;
}

const std::pair<const std::string, const value::Value*>*
    EventValue::IteratorImpl::operator->() const {
    return &(**this);
}

}
}
