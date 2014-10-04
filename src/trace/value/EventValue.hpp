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
#ifndef _TIBEE_TRACE_VALUE_EVENTVALUE_HPP
#define _TIBEE_TRACE_VALUE_EVENTVALUE_HPP

#include <babeltrace/ctf/events.h>

#include "base/BasicTypes.hpp"
#include "trace/BasicTypes.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace trace
{

// Forward declaration.
class EventValueFactory;

class EventValue :
    public value::StructValueBase
{
    friend class TraceSetIterator;

public:
    // Offsets of the top-level fields of every event.
    static const char kNameField[];
    static const char kTimestampField[];
    static const char kFieldsField[];
    static const char kContextField[];
    static const char kStreamPacketContextField[];

    // Names of the top-level fields of every event.
    static const size_t kNameFieldOffset;
    static const size_t kTimestampFieldOffset;
    static const size_t kFieldsFieldOffset;
    static const size_t kContextFieldOffset;
    static const size_t kStreamPacketContextFieldOffset;
    static const size_t kNumFields;

    using value::StructValueBase::GetField;

    // Overridden from StructValueBase:
    virtual size_t Length() const override;
    virtual bool HasField(const std::string& name) const override;
    virtual const value::Value* GetField(const std::string& name) const override;
    virtual const Value* at(size_t index) const override;
    virtual value::StructValueBase::Iterator fields_begin() const override;
    virtual value::StructValueBase::Iterator fields_end() const override;

    /**
     * Returns the event name.
     *
     * @returns Event name
     */
    const char* getName() const;

    /**
     * Returns a copy of the event name.
     *
     * @returns Event name copy
     */
    std::string getNameStr() const;

    /**
     * Returns the cycle count of this event.
     *
     * @returns Cycle count
     */
    trace_cycles_t getCycles() const;

    /**
     * Returns the event timestamp.
     *
     * @returns Event timestamp
     */
    timestamp_t getTimestamp() const;

    /**
     * Returns the event fields dictionary.
     *
     * The returned reference is only valid while this event is valid.
     *
     * If the event has no fields, this method returns a null event
     * value.
     *
     * Once this dictionary is created, a copy of the pointer is cached
     * by the owning event. The copy is kept as long as this event remains
     * valid.
     *
     * @returns Event fields dictionary or null event value if not available
     */
    const value::Value* getFields() const;

    /**
     * Returns the value of a field.
     *
     * @param name the name of the field to get.
     * @returns the value of the field.
     */
    const value::Value* getEventField(const std::string& name) const;

    /**
     * Returns the event context dictionary.
     *
     * The returned reference is only valid while this event is valid.
     *
     * If the event has no fields, this method returns a null event
     * value.
     *
     * Once this dictionary is created, a copy of the pointer is cached
     * by the owning event. The copy is kept as long as this event remains
     * valid.
     *
     * @returns Event context or null event value if not available
     */
    const value::Value* getContext() const;

    /**
     * Returns the stream event context dictionary.
     *
     * The returned reference is only valid while this event is valid.
     *
     * If the event has no fields, this method returns a null event
     * value.
     *
     * Once this dictionary is created, a copy of the pointer is cached
     * by the owning event. The copy is kept as long as this event remains
     * valid.
     *
     * @returns Stream event context or null event value if not available
     */
    const value::Value* getStreamEventContext() const;

    /**
     * Returns the stream packet context dictionary.
     *
     * The returned reference is only valid while this event is valid.
     *
     * If the event has no fields, this method returns a null event
     * value.
     *
     * Once this dictionary is created, a copy of the pointer is cached
     * by the owning event. The copy is kept as long as this event remains
     * valid.
     *
     * @returns Stream packet context or null event value if not available
     */
    const value::Value* getStreamPacketContext() const;

     /**
     * Returns this event's numeric ID.
     *
     * @returns Event numeric ID
     */
    const event_id_t getId() const
    {
        return _id;
    }

    /**
     * Returns this event's trace numeric ID.
     *
     * @returns Numeric ID of trace this event is in
     */
    const trace_id_t getTraceId() const
    {
        return _traceId;
    }

private:
    // Implementation of a struct iterator.
    class IteratorImpl :
        public StructValueBase::IteratorImpl {
    public:
        IteratorImpl(const EventValue* eventValue, size_t index);

        virtual value::StructValueBase::IteratorImpl& operator++() override;
        virtual bool operator==(
            const value::StructValueBase::IteratorImpl& other) const override;
        virtual bool operator!=(
            const value::StructValueBase::IteratorImpl& other) const override;
        virtual const std::pair<const std::string, const value::Value*>&
            operator*() const override;
        virtual const std::pair<const std::string, const value::Value*>*
            operator->() const override;

    private:
        const EventValue* _eventValue;
        size_t _currentIndex;
        mutable std::unique_ptr<
            std::pair<const std::string, const value::Value*>>
                _currentPair;
    };

private:
    EventValue(const EventValueFactory* valueFactory);
    const value::Value* getTopLevelScope(::bt_ctf_scope topLevelScope) const;
    void setPrivateEvent(::bt_ctf_event* btEvent);

    ::bt_ctf_event* _btEvent;
    const EventValueFactory* _valueFactory;
    mutable value::StringValue _name;
    mutable value::ULongValue _ts;
    mutable const value::Value* _fieldsDict;
    mutable const value::Value* _contextDict;
    mutable const value::Value* _streamEventContextDict;
    mutable const value::Value* _streamPacketContextDict;
    event_id_t _id;
    trace_id_t _traceId;

    // Empty dictionary value.
    value::StructValue _emptyStruct;
};

}
}

#endif  // _TIBEE_TRACE_VALUE_EVENTVALUE_HPP
