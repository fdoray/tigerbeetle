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
#include "trace/value/ArrayEventValue.hpp"

#include <cstddef>
#include <sstream>
#include <vector>
#include <babeltrace/ctf/events.h>

#include <iostream>
#include <babeltrace/types.h>

#include "trace/EventValueFactory.hpp"

namespace tibee
{
namespace trace
{

ArrayEventValue::ArrayEventValue(const ::bt_definition* def, const ::bt_ctf_event* ev,
                                 const EventValueFactory* valueFactory) :
    _btDef {def},
    _btEvent {ev},
    _btFieldList {nullptr},
    _size {0},
    _valueFactory {valueFactory}
{
    this->buildCache();
}

void ArrayEventValue::buildCache()
{
    unsigned int count;

    auto ret = ::bt_ctf_get_field_list(_btEvent, _btDef, &_btFieldList, &count);

    if (ret == 0) {
        _size = count;
    }
}

std::size_t ArrayEventValue::Length() const
{
    return _size;
}

const value::Value* ArrayEventValue::at(size_t index) const
{
    // this should work for both CTF array and sequence
    auto itemDef = _btFieldList[index];

    return _valueFactory->buildEventValue(itemDef, _btEvent);
}

value::ArrayValueBase::Iterator ArrayEventValue::begin() const
{
    return value::ArrayValueBase::Iterator(new IteratorImpl(this, 0));
}

value::ArrayValueBase::Iterator ArrayEventValue::end() const
{
    return value::ArrayValueBase::Iterator(new IteratorImpl(this, _size));
}

ArrayEventValue::IteratorImpl::IteratorImpl(
    const ArrayEventValue* arrayValue, size_t index)
    : _arrayValue {arrayValue},
      _currentIndex {index} {
}

value::ArrayValueBase::IteratorImpl&
    ArrayEventValue::IteratorImpl::operator++() {
  ++_currentIndex;
  return *this;
}

bool ArrayEventValue::IteratorImpl::operator==(
    const ArrayValueBase::IteratorImpl& other) const {
  auto other_cast = reinterpret_cast<const ArrayEventValue::IteratorImpl*>(
      std::addressof(other));
  return _currentIndex == other_cast->_currentIndex;
}

bool ArrayEventValue::IteratorImpl::operator!=(
    const ArrayValueBase::IteratorImpl& other) const {
  auto other_cast = reinterpret_cast<const ArrayEventValue::IteratorImpl*>(
      std::addressof(other));
  return _currentIndex != other_cast->_currentIndex;
}

const value::Value& ArrayEventValue::IteratorImpl::operator*() const {
  return *_arrayValue->at(_currentIndex);
}

const value::Value* ArrayEventValue::IteratorImpl::operator->() const {
  return _arrayValue->at(_currentIndex);
}

}
}
