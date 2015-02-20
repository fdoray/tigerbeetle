// Copyright (c) 2014 The LibTrace Authors.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the <organization> nor the
//     names of its contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "value/Value.hpp"

#include <limits>

#include "base/StringUtils.hpp"
#include "value/Utils.hpp"
#include "value/ex/InvalidConversion.hpp"

namespace tibee {
namespace value {

bool Value::AsInteger(int32_t* value) const {
  assert(value != nullptr);

  switch (GetType()) {
    case VALUE_BOOL: {
      *value = BoolValue::GetValue(this);
      return true;
    }
    case VALUE_CHAR: {
      *value = CharValue::GetValue(this);
      return true;
    }
    case VALUE_UCHAR: {
      *value = UCharValue::GetValue(this);
      return true;
    }
    case VALUE_SHORT: {
      *value = ShortValue::GetValue(this);
      return true;
    }
    case VALUE_USHORT: {
      *value = UShortValue::GetValue(this);
      return true;
    }
    case VALUE_INT: {
      *value = IntValue::GetValue(this);
      return true;
    }
    case VALUE_UINT: {
      uint32_t raw_value = UIntValue::GetValue(this);
      if (raw_value > static_cast<uint32_t>(IntValue::MaxValue()))
        return false;
      *value = static_cast<int32_t>(raw_value);
      return true;
    }
    case VALUE_LONG: {
      int64_t raw_value = LongValue::GetValue(this);
      if (raw_value > static_cast<int64_t>(IntValue::MaxValue()) ||
          raw_value < static_cast<int64_t>(IntValue::MinValue())) {
         return false;
      }
      *value = static_cast<int32_t>(raw_value);
      return true;
    }
    case VALUE_ULONG: {
      uint64_t raw_value = ULongValue::GetValue(this);
      if (raw_value > static_cast<uint64_t>(IntValue::MaxValue()))
        return false;
      *value = static_cast<int32_t>(raw_value);
      return true;
    }
    default:
      return false;
  }
}

bool Value::AsUInteger(uint32_t* value) const {
  assert(value != nullptr);

  switch (GetType()) {
    case VALUE_BOOL: {
      *value = BoolValue::GetValue(this);
      return true;
    }
    case VALUE_CHAR: {
      int32_t raw_value = CharValue::GetValue(this);
      if (raw_value < 0)
        return false;
      *value = static_cast<uint32_t>(raw_value);
      return true;
    }
    case VALUE_UCHAR: {
      *value = UCharValue::GetValue(this);
      return true;
    }
    case VALUE_SHORT: {
      int32_t raw_value = ShortValue::GetValue(this);
      if (raw_value < 0)
        return false;
      *value = static_cast<uint32_t>(raw_value);
      return true;
    }
    case VALUE_USHORT: {
      *value = UShortValue::GetValue(this);
      return true;
    }
    case VALUE_INT: {
       int32_t raw_value = IntValue::GetValue(this);
       if (raw_value < 0)
         return false;
      *value = static_cast<uint32_t>(raw_value);
      return true;
    }
    case VALUE_UINT: {
      *value = UIntValue::GetValue(this);
      return true;
    }
    case VALUE_LONG: {
      int64_t raw_value = LongValue::GetValue(this);
      if (raw_value < 0)
        return false;
      if (raw_value > static_cast<int64_t>(UIntValue::MaxValue()))
        return false;
      *value = static_cast<uint32_t>(raw_value);
      return true;
    }
    case VALUE_ULONG: {
      uint64_t raw_value = ULongValue::GetValue(this);
      if (raw_value > static_cast<uint64_t>(UIntValue::MaxValue()))
        return false;
      *value = static_cast<uint32_t>(raw_value);
      return true;
    }
    default:
      return false;
  }
}

bool Value::AsLong(int64_t* value) const {
  assert(value != nullptr);

  switch (GetType()) {
    case VALUE_BOOL: {
      *value = BoolValue::GetValue(this);
      return true;
    }
    case VALUE_CHAR: {
      *value = CharValue::GetValue(this);
      return true;
    }
    case VALUE_UCHAR: {
      *value = UCharValue::GetValue(this);
      return true;
    }
    case VALUE_SHORT: {
      *value = ShortValue::GetValue(this);
      return true;
    }
    case VALUE_USHORT: {
      *value = UShortValue::GetValue(this);
      return true;
    }
    case VALUE_INT: {
      *value = IntValue::GetValue(this);
      return true;
    }
    case VALUE_UINT: {
      *value = UIntValue::GetValue(this);
      return true;
    }
    case VALUE_LONG: {
      *value = LongValue::GetValue(this);
      return true;
    }
    case VALUE_ULONG: {
      uint64_t uvalue = ULongValue::GetValue(this);
      if (uvalue > static_cast<uint64_t>(LongValue::MaxValue()))
        return false;
      *value = static_cast<int64_t>(uvalue);
      return true;
    }
    default:
      return false;
  }
}

bool Value::AsULong(uint64_t* value) const {
  assert(value != nullptr);

  switch (GetType()) {
    case VALUE_BOOL: {
      *value = BoolValue::GetValue(this);
      return true;
    }
    case VALUE_CHAR: {
      int32_t raw_value = CharValue::GetValue(this);
      if (raw_value < 0)
        return false;
      *value = static_cast<uint64_t>(raw_value);
      return true;
    }
    case VALUE_UCHAR: {
      *value = UCharValue::GetValue(this);
      return true;
    }
    case VALUE_SHORT: {
      int32_t raw_value = ShortValue::GetValue(this);
      if (raw_value < 0)
        return false;
      *value = static_cast<uint64_t>(raw_value);
      return true;
    }
    case VALUE_USHORT: {
      *value = UShortValue::GetValue(this);
      return true;
    }
    case VALUE_INT: {
      int32_t raw_value = IntValue::GetValue(this);
      if (raw_value < 0)
        return false;
      *value = static_cast<uint64_t>(raw_value);
      return true;
    }
    case VALUE_UINT: {
      *value = UIntValue::GetValue(this);
      return true;
    }
    case VALUE_LONG: {
      int64_t raw_value = LongValue::GetValue(this);
      if (raw_value < 0)
        return false;
      *value = static_cast<uint64_t>(raw_value);
      return true;
    }
    case VALUE_ULONG: {
      *value = ULongValue::GetValue(this);
      return true;
    }
    default:
      return false;
  }
}

bool Value::AsFloating(double* value) const {
  assert(value != nullptr);

  switch (GetType()) {
    case VALUE_FLOAT: {
      *value = FloatValue::GetValue(this);
      return true;
    }
    case VALUE_DOUBLE: {
      *value = DoubleValue::GetValue(this);
      return true;
    }
    default:
      return false;
  }
}

bool Value::AsString(std::string* value) const {
  assert(value != nullptr);

  switch (GetType()) {
    case VALUE_STRING: {
      *value = StringValue::GetValue(this);
      return true;
    }
    case VALUE_WSTRING: {
      *value = base::WStringToString(WStringValue::GetValue(this));
      return true;
    }
    default:
      return false;
  }
}

bool Value::AsWString(std::wstring* value) const {
  assert(value != nullptr);

  switch (GetType()) {
    case VALUE_STRING: {
      *value = base::StringToWString(StringValue::GetValue(this));
      return true;
    }
    case VALUE_WSTRING: {
      *value = WStringValue::GetValue(this);
      return true;
    }
    default:
      return false;
  }
}

int32_t Value::AsInteger() const
{
  int32_t value = 0;
  if (!AsInteger(&value))
    throw ex::InvalidConversion(std::string("The value cannot be converted to an integer. ") + value::ToString(this));
  return value;
}

uint32_t Value::AsUInteger() const
{
  uint32_t value = 0;
  if (!AsUInteger(&value))
    throw ex::InvalidConversion(std::string("The value cannot be converted to an unsigned integer. ") + value::ToString(this));
  return value;
}

int64_t Value::AsLong() const
{
  int64_t value = 0;
  if (!AsLong(&value))
    throw ex::InvalidConversion(std::string("The value cannot be converted to a long. ") + value::ToString(this));
  return value;
}

uint64_t Value::AsULong() const
{
  uint64_t value = 0;
  if (!AsULong(&value))
    throw ex::InvalidConversion(std::string("The value cannot be converted to an unsigned long. ") + value::ToString(this));
  return value;
}

double Value::AsFloating() const
{
  double value = 0;
  if (!AsFloating(&value))
    throw ex::InvalidConversion(std::string("The value cannot be converted to a float. ") + value::ToString(this));
  return value;
}

std::string Value::AsString() const
{
  std::string value;
  if (!AsString(&value))
    throw ex::InvalidConversion(std::string("The value cannot be converted to a string. ") + value::ToString(this));
  return value;
}

std::wstring Value::AsWString() const
{
  std::wstring value;
  if (!AsWString(&value))
    throw ex::InvalidConversion("The value cannot be converted to a wide string.");
  return value;
}

bool Value::AreEqual(const Value* left, const Value* right) {
  if (left == nullptr || right == nullptr)
    return left == right;

  return left->Equals(right);
}

bool Value::GetField(const std::string& name,
                     const Value** value) const {
  assert(value != nullptr);
  *value = GetField(name);
  return *value != nullptr;
}

template<class T, int TYPE>
ValueType ScalarValue<T, TYPE>::GetType() const {
  return static_cast<ValueType>(TYPE);
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::IsScalar() const {
  return true;
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::IsAggregate() const {
  return false;
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::IsInteger() const {
  return std::numeric_limits<T>::is_specialized &&
         std::numeric_limits<T>::is_integer;
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::IsSigned() const {
  return std::numeric_limits<T>::is_specialized &&
         (!std::numeric_limits<T>::is_integer ||
          std::numeric_limits<T>::is_signed);
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::IsFloating() const {
  return std::numeric_limits<T>::is_specialized &&
         !std::numeric_limits<T>::is_integer;
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::IsString() const {
  return GetType() == VALUE_STRING || GetType() == VALUE_WSTRING;
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::Equals(const Value* value) const {
  if (value == nullptr)
    return false;

  if (!ScalarValue<T, TYPE>::InstanceOf(value))
    return false;
  if (ScalarValue<T, TYPE>::Cast(value)->GetValue() != GetValue())
    return false;
  return true;
}

template<class T, int TYPE>
Value::UP ScalarValue<T, TYPE>::Copy() const {
  Value::UP copy {new ScalarValue<T, TYPE>(value_)};
  return copy;
}

template<class T, int TYPE>
const T& ScalarValue<T, TYPE>::GetValue() const {
  return value_;
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::InstanceOf(const Value* value) {
  if (value == nullptr)
    return false;
  return value->GetType() == TYPE;
}

template<class T, int TYPE>
const ScalarValue<T, TYPE>* ScalarValue<T, TYPE>::Cast(const Value* value) {
  assert(value != nullptr);
  assert(value->GetType() == TYPE);
  return reinterpret_cast<const SelfType*>(value);
}

template<class T, int TYPE>
ScalarValue<T, TYPE>* ScalarValue<T, TYPE>::Cast(Value* value) {
  assert(value != nullptr);
  assert(value->GetType() == TYPE);
  return reinterpret_cast<SelfType*>(value);
}

template<class T, int TYPE>
const T& ScalarValue<T, TYPE>::GetValue(const Value* value) {
  assert(value != nullptr);
  return Cast(value)->GetValue();
}

template<class T, int TYPE>
bool ScalarValue<T, TYPE>::GetValue(const Value* value, T* dst) {
  assert(value != nullptr);
  assert(dst != nullptr);
  if (!InstanceOf(value))
    return false;
  *dst = Cast(value)->GetValue();
  return true;
}

template<class T, int TYPE>
T ScalarValue<T, TYPE>::MinValue() {
  return std::numeric_limits<T>::min();
}

template<class T, int TYPE>
T ScalarValue<T, TYPE>::MaxValue() {
  return std::numeric_limits<T>::max();
}

template<int TYPE>
ValueType AggregateValue<TYPE>::GetType() const {
  return static_cast<ValueType>(TYPE);
}

template<int TYPE>
bool AggregateValue<TYPE>::IsScalar() const {
  return false;
}

template<int TYPE>
bool AggregateValue<TYPE>::IsAggregate() const {
  return true;
}

template<int TYPE>
bool AggregateValue<TYPE>::IsInteger() const {
  return false;
}

template<int TYPE>
bool AggregateValue<TYPE>::IsSigned() const {
  return false;
}

template<int TYPE>
bool AggregateValue<TYPE>::IsFloating() const {
  return false;
}

template<int TYPE>
bool AggregateValue<TYPE>::IsString() const {
  return false;
}

ArrayValueBase::ArrayValueBase() {
}

ArrayValueBase::~ArrayValueBase() {
}

bool ArrayValueBase::IsEmpty() const {
  return Length() == 0;
}

const Value* ArrayValueBase::operator[](size_t index) const {
  return at(index);
}

bool ArrayValueBase::GetElementAsInteger(size_t index, int32_t* value) const {
  assert(value != nullptr);
  if (index >= Length())
    return false;
  return at(index)->AsInteger(value);
}

bool ArrayValueBase::GetElementAsUInteger(size_t index, uint32_t* value) const {
  assert(value != nullptr);
  if (index >= Length())
    return false;
  return at(index)->AsUInteger(value);
}

bool ArrayValueBase::GetElementAsLong(size_t index, int64_t* value) const {
  assert(value != nullptr);
  if (index >= Length())
    return false;
  return at(index)->AsLong(value);
}

bool ArrayValueBase::GetElementAsULong(size_t index, uint64_t* value) const {
  assert(value != nullptr);
  if (index >= Length())
    return false;
  return at(index)->AsULong(value);
}

bool ArrayValueBase::GetElementAsFloating(size_t index, double* value) const {
  assert(value != nullptr);
  if (index >= Length())
    return false;
  return at(index)->AsFloating(value);
}

bool ArrayValueBase::GetElementAsString(size_t index, std::string* value) const {
  assert(value != nullptr);
  if (index >= Length())
    return false;
  return at(index)->AsString(value);
}

bool ArrayValueBase::GetElementAsWString(size_t index, std::wstring* value) const {
  assert(value != nullptr);
  if (index >= Length())
    return false;
  return at(index)->AsWString(value);
}

bool ArrayValueBase::Equals(const Value* value) const {
  if (value == nullptr)
    return false;

  if (!ArrayValueBase::InstanceOf(value))
    return false;

  const ArrayValueBase* array = ArrayValueBase::Cast(value);
  if (array->Length() != Length())
    return false;

  size_t length = Length();

  for (size_t i = 0; i < length; ++i) {
    if (!at(i)->Equals(array->at(i)))
      return false;
  }

  return true;
}

Value::UP ArrayValueBase::Copy() const {
  ArrayValue::UP copy {new ArrayValue};

  auto it = begin();
  auto it_end = end();

  for (; it != it_end; ++it)
    copy->Append(it->Copy());

  return std::move(copy);
}

bool ArrayValueBase::InstanceOf(const Value* value) {
  if (value == nullptr)
    return false;
  return value->GetType() == VALUE_ARRAY;
}

const ArrayValueBase* ArrayValueBase::Cast(const Value* value) {
  assert(value != nullptr);
  assert(value->GetType() == VALUE_ARRAY);
  return reinterpret_cast<const ArrayValue*>(value);
}

ArrayValueBase::Iterator::Iterator(ArrayValueBase::IteratorImpl* impl)
    : impl_(impl) {
}

ArrayValueBase::Iterator& ArrayValueBase::Iterator::operator++() {
  ++(*impl_);
  return *this;
}

bool ArrayValueBase::Iterator::operator==(
    const ArrayValueBase::Iterator& other) const {
  return *impl_ == *other.impl_;
}

bool ArrayValueBase::Iterator::operator!=(
    const ArrayValueBase::Iterator& other) const {
  return *impl_ != *other.impl_;
}

const Value& ArrayValueBase::Iterator::operator*() const {
  return **impl_;
}

const Value* ArrayValueBase::Iterator::operator->() const {
  return &(**impl_);
}

ArrayValue::ArrayValue() {
}

ArrayValue::~ArrayValue() {
  for (Values::iterator it = values_.begin(); it != values_.end(); ++it)
    delete *it;
}

size_t ArrayValue::Length() const {
  return values_.size();
}

const Value* ArrayValue::at(size_t index) const {
  return values_.at(index);
}

ArrayValueBase::Iterator ArrayValue::begin() const {
  return ArrayValueBase::Iterator(new IteratorImpl(values_.begin()));
}

ArrayValueBase::Iterator ArrayValue::end() const {
  return ArrayValueBase::Iterator(new IteratorImpl(values_.end()));
}

Value* ArrayValue::operator[](size_t index) {
  return at(index);
}

Value* ArrayValue::at(size_t index) {
  return values_.at(index);
}

void ArrayValue::Append(std::unique_ptr<Value> value) {
  assert(value.get() != nullptr);
  values_.push_back(value.release());
}

ArrayValue::IteratorImpl::IteratorImpl(ArrayValue::const_iterator it)
    : it_(it) {
}

ArrayValueBase::IteratorImpl&
    ArrayValue::IteratorImpl::operator++() {
  ++it_;
  return *this;
}

bool ArrayValue::IteratorImpl::operator==(
    const ArrayValueBase::IteratorImpl& other) const {
  auto other_cast = reinterpret_cast<const ArrayValue::IteratorImpl*>(
      std::addressof(other));
  return it_ == other_cast->it_;
}

bool ArrayValue::IteratorImpl::operator!=(
    const ArrayValueBase::IteratorImpl& other) const {
  auto other_cast = reinterpret_cast<const ArrayValue::IteratorImpl*>(
      std::addressof(other));
  return it_ != other_cast->it_;
}

const Value& ArrayValue::IteratorImpl::operator*() const {
  return **it_;
}

const Value* ArrayValue::IteratorImpl::operator->() const {
  return &**it_;
}

StructValueBase::StructValueBase() {
}

StructValueBase::~StructValueBase() {
}

const Value* StructValueBase::operator[](size_t index) const {
  return at(index);
}

bool StructValueBase::GetFieldAsInteger(
    const std::string& name, int32_t* value) const {
  assert(value != nullptr);
  const Value* field = nullptr;
  if (!GetField(name, &field))
    return false;
  return field->AsInteger(value);
}

bool StructValueBase::GetFieldAsUInteger(
    const std::string& name, uint32_t* value) const {
  assert(value != nullptr);
  const Value* field = nullptr;
  if (!GetField(name, &field))
    return false;
  return field->AsUInteger(value);
}

bool StructValueBase::GetFieldAsLong(const std::string& name,
                                     int64_t* value) const {
  assert(value != nullptr);
  const Value* field = nullptr;
  if (!GetField(name, &field))
    return false;
  return field->AsLong(value);
}

bool StructValueBase::GetFieldAsULong(
    const std::string& name, uint64_t* value) const {
  assert(value != nullptr);
  const Value* field = nullptr;
  if (!GetField(name, &field))
    return false;
  return field->AsULong(value);
}

bool StructValueBase::GetFieldAsFloating(
    const std::string& name, double* value) const {
  assert(value != nullptr);
  const Value* field = nullptr;
  if (!GetField(name, &field))
    return false;
  return field->AsFloating(value);
}

bool StructValueBase::GetFieldAsString(
    const std::string& name, std::string* value) const {
  assert(value != nullptr);
  const Value* field = nullptr;
  if (!GetField(name, &field))
    return false;
  return field->AsString(value);
}

bool StructValueBase::GetFieldAsWString(
    const std::string& name, std::wstring* value) const {
  assert(value != nullptr);
  const Value* field = nullptr;
  if (!GetField(name, &field))
    return false;
  return field->AsWString(value);
}

bool StructValueBase::Equals(const Value* value) const {
  if (value == nullptr)
    return false;

  if (!StructValueBase::InstanceOf(value))
    return false;

  const StructValueBase* strct = StructValueBase::Cast(value);

  auto left = fields_begin();
  auto right = strct->fields_begin();
  while (left != fields_end() && right != strct->fields_end()) {
    if (left->first.compare(right->first) != 0)
      return false;
    if (!left->second->Equals(right->second))
      return false;

    ++left;
    ++right;
  }

  if (left != fields_end() || right != strct->fields_end())
    return false;

  return true;
}

Value::UP StructValueBase::Copy() const {
  StructValue::UP copy {new StructValue};

  auto it = fields_begin();
  auto it_end = fields_end();

  for (; it != it_end; ++it)
    copy->AddField(it->first, it->second->Copy());

  return std::move(copy);
}

bool StructValueBase::InstanceOf(const Value* value) {
  if (value == nullptr)
    return false;
  return value->GetType() == VALUE_STRUCT;
}

const StructValueBase* StructValueBase::Cast(const Value* value) {
  assert(value != nullptr);
  assert(value->GetType() == VALUE_STRUCT);
  return reinterpret_cast<const StructValueBase*>(value);
}

StructValueBase::Iterator::Iterator(StructValueBase::IteratorImpl* impl)
    : impl_(impl) {
}

StructValueBase::Iterator& StructValueBase::Iterator::operator++() {
  ++(*impl_);
  return *this;
}

bool StructValueBase::Iterator::operator==(
    const StructValueBase::Iterator& other) const {
  return *impl_ == *other.impl_;
}

bool StructValueBase::Iterator::operator!=(
    const StructValueBase::Iterator& other) const {
  return *impl_ != *other.impl_;
}

const std::pair<const std::string, const Value*>&
    StructValueBase::Iterator::operator*() const {
  return **impl_;
}

const std::pair<const std::string, const Value*>*
    StructValueBase::Iterator::operator->() const {
  return &(**impl_);
}

StructValue::StructValue() {
}

StructValue::~StructValue() {
  for (auto it = fields_map_.begin(); it != fields_map_.end(); ++it)
    delete it->second;
}

size_t StructValue::Length() const {
  return fields_.size();
}

bool StructValue::HasField(const std::string& name) const {
  return fields_map_.find(name) != fields_map_.end();
}

const Value* StructValue::GetField(const std::string& name) const {
  auto look = fields_map_.find(name);
  if (look == fields_map_.end())
    return nullptr;
  return look->second;
}

const Value* StructValue::at(size_t index) const {
  if (index >= fields_.size())
    return nullptr;
  return fields_[index]->second;
}

StructValueBase::Iterator StructValue::fields_begin() const {
  return StructValueBase::Iterator(new IteratorImpl(fields_.begin()));
}

StructValueBase::Iterator StructValue::fields_end() const {
  return StructValueBase::Iterator(new IteratorImpl(fields_.end()));
}

bool StructValue::AddField(const std::string& name,
                           std::unique_ptr<Value> value) {
  if (HasField(name))
    return false;
  Value* raw_value = value.release();
  auto res = fields_map_.insert(std::make_pair(name, raw_value));
  fields_.push_back(&*res.first);
  return true;
}

StructValue::IteratorImpl::IteratorImpl(StructValue::const_iterator it)
    : it_(it) {
}

StructValueBase::IteratorImpl&
    StructValue::IteratorImpl::operator++() {
  ++it_;
  currentPair_.reset(nullptr);
  return *this;
}

bool StructValue::IteratorImpl::operator==(
    const StructValueBase::IteratorImpl& other) const {
  auto other_cast = reinterpret_cast<const StructValue::IteratorImpl*>(
      std::addressof(other));
  return it_ == other_cast->it_;
}

bool StructValue::IteratorImpl::operator!=(
    const StructValueBase::IteratorImpl& other) const {
  auto other_cast = reinterpret_cast<const StructValue::IteratorImpl*>(
      std::addressof(other));
  return it_ != other_cast->it_;
}

const std::pair<const std::string, const Value*>&
    StructValue::IteratorImpl::operator*() const {
  if (currentPair_.get() == nullptr) {
    currentPair_.reset(
      new std::pair<const std::string, const Value*> {
        (**it_).first,
        (**it_).second
      });
  }
  return *currentPair_;
}

const std::pair<const std::string, const Value*>*
    StructValue::IteratorImpl::operator->() const {
  return &(**this);
}

// Force a template instantiation in this compilation unit. This must be at
// the end of the file because only defined methods are instantiated.
template class ScalarValue<bool, VALUE_BOOL>;
template class ScalarValue<int8_t, VALUE_CHAR>;
template class ScalarValue<uint8_t, VALUE_UCHAR>;
template class ScalarValue<int16_t, VALUE_SHORT>;
template class ScalarValue<uint16_t, VALUE_USHORT>;
template class ScalarValue<int32_t, VALUE_INT>;
template class ScalarValue<uint32_t, VALUE_UINT>;
template class ScalarValue<int64_t, VALUE_LONG>;
template class ScalarValue<uint64_t, VALUE_ULONG>;
template class ScalarValue<std::string, VALUE_STRING>;
template class ScalarValue<std::wstring, VALUE_WSTRING>;
template class ScalarValue<float, VALUE_FLOAT>;
template class ScalarValue<double, VALUE_DOUBLE>;

}  // namespace value
}  // namespace tibee
