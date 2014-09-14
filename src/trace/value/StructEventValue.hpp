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
#ifndef _TIBEE_TRACE_DICTEVENTVALUE_HPP
#define _TIBEE_TRACE_DICTEVENTVALUE_HPP

#include <cstddef>
#include <map>
#include <babeltrace/ctf/events.h>

#include "value/Value.hpp"

namespace tibee
{
namespace trace
{

// Forward declaration.
class EventValueFactory;

/**
 * Event value carrying an dictionary of values.
 *
 * @author Philippe Proulx
 */
class StructEventValue :
    public value::StructValueBase
{
public:
    /**
     * Builds a dictionary value out of a field definition.
     *
     * @param def          BT field definition
     * @param ev           BT event
     * @param valueFactory Value factory used to create other event values
     */
    StructEventValue(const ::bt_definition* def, const ::bt_ctf_event* ev,
                     const EventValueFactory* valueFactory);

    using value::StructValueBase::GetField;

    // Overridden from value::StructValueBase:
    virtual size_t Length() const override;
    virtual bool HasField(const std::string& name) const override;
    virtual const Value* GetField(const std::string& name) const override;
    virtual const Value* at(size_t index) const override;
    virtual value::StructValueBase::Iterator fields_begin() const override;
    virtual value::StructValueBase::Iterator fields_end() const override;

    /**
     * Returns the key name at index \p index without checking
     * bounds.
     *
     * @param index Index of key of which to get the name
     * @returns     Name of key at index \p index
     */
    const char* getKeyName(std::size_t index) const;

    /**
     * Returns a copy of the key name at index \p index without
     * checking bounds.
     *
     * @param index Index of key of which to get a copy of the name
     * @returns     Copy of name of key at index \p index
     */
    std::string getKeyNameStr(std::size_t index) const;

private:
    // Implementation of a struct iterator.
    class IteratorImpl :
        public StructValueBase::IteratorImpl {
    public:
        IteratorImpl(const StructEventValue* structValue, size_t index);

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
        const StructEventValue* _structValue;
        size_t _currentIndex;
        mutable std::unique_ptr<
            std::pair<const std::string, const value::Value*>>
                _currentPair;
    };

private:
    void buildCache();

private:
    const ::bt_definition* _btDef;
    const ::bt_declaration* _btDecl;
    const ::bt_ctf_event* _btEvent;
    ::bt_definition const* const* _btFieldList;
    std::size_t _size;
    const EventValueFactory* _valueFactory;
};

}
}

#endif // _TIBEE_TRACE_DICTEVENTVALUE_HPP