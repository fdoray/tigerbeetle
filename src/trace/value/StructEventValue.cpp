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

#include "trace/value/StructEventValue.hpp"

#include <cstddef>
#include <cstring>
#include <sstream>
#include <map>
#include <babeltrace/ctf/events.h>

#include "trace/EventValueFactory.hpp"

namespace tibee
{
namespace trace
{

StructEventValue::StructEventValue(const ::bt_definition* def,
                                   const ::bt_ctf_event* ev,
                                   const EventValueFactory* valueFactory) :
    _btDef {def},
    _btEvent {ev},
    _btFieldList {nullptr},
    _size {0},
    _valueFactory {valueFactory}
{
    this->buildCache();
}

void StructEventValue::buildCache()
{
    _btDecl = ::bt_ctf_get_decl_from_def(_btDef);

    unsigned int count;

    auto ret = ::bt_ctf_get_field_list(_btEvent, _btDef, &_btFieldList, &count);

    if (ret == 0) {
        _size = count;
    }
}

size_t StructEventValue::Length() const {
    return _size;
}

bool StructEventValue::HasField(const std::string& name) const {
    // linear search (not efficient)
    for (std::size_t x = 0; x < _size; ++x) {
        auto keyName = this->getKeyName(x);

        if (std::strcmp(keyName, name.c_str()) == 0) {
            // match!
            return true;
        }
    }

    return false;
}

const value::Value* StructEventValue::GetField(const std::string& name) const {
    // linear search (not efficient)
    for (std::size_t x = 0; x < _size; ++x) {
        auto keyName = this->getKeyName(x);

        if (std::strcmp(keyName, name.c_str()) == 0) {
            return this->at(x);
        }
    }

    return nullptr;
}

const value::Value* StructEventValue::at(size_t index) const
{
    auto itemDef = _btFieldList[index];
    return _valueFactory->buildEventValue(itemDef, _btEvent);
}

value::StructValueBase::Iterator StructEventValue::fields_begin() const
{
    return value::StructValueBase::Iterator(new IteratorImpl(this, 0));
}

value::StructValueBase::Iterator StructEventValue::fields_end() const
{
    return value::StructValueBase::Iterator(new IteratorImpl(this, _size));
}

const char* StructEventValue::getKeyName(std::size_t index) const
{
    if (!_btFieldList) {
        return nullptr;
    }

    auto def = _btFieldList[index];

    return ::bt_ctf_field_name(def);
}

std::string StructEventValue::getKeyNameStr(std::size_t index) const
{
    auto name = this->getKeyName(index);

    if (!name) {
        name = "";
    }

    return std::string {name};
}

StructEventValue::IteratorImpl::IteratorImpl(
    const StructEventValue* structValue, size_t index)
    : _structValue {structValue},
      _currentIndex(index)
{
}

value::StructValueBase::IteratorImpl&
    StructEventValue::IteratorImpl::operator++()
{
    ++_currentIndex;
    _currentPair.reset(nullptr);
    return *this;
}

bool StructEventValue::IteratorImpl::operator==(
    const StructValueBase::IteratorImpl& other) const
{
    auto other_cast = reinterpret_cast<const StructEventValue::IteratorImpl*>(
        std::addressof(other));
    return _currentIndex == other_cast->_currentIndex;
}

bool StructEventValue::IteratorImpl::operator!=(
    const StructValueBase::IteratorImpl& other) const {
    auto other_cast = reinterpret_cast<const StructEventValue::IteratorImpl*>(
        std::addressof(other));
    return _currentIndex != other_cast->_currentIndex;
}

const std::pair<const std::string, const value::Value*>&
    StructEventValue::IteratorImpl::operator*() const {
    if (_currentPair.get() == nullptr) {
        _currentPair.reset(
            new std::pair<const std::string, const value::Value*> {
                _structValue->getKeyName(_currentIndex),
                _structValue->at(_currentIndex)
            });
    }
    return *_currentPair;
}

const std::pair<const std::string, const value::Value*>*
    StructEventValue::IteratorImpl::operator->() const {
    return &(**this);
}

}
}
