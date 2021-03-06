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

#include "value/Utils.hpp"

#include <assert.h>
#include <sstream>

#include "value/Value.hpp"

namespace tibee {
namespace value {

namespace {

bool ToString(const Value* value, size_t indent, std::stringstream* result) {
  assert(result != nullptr);

  if (value == nullptr) {
    *result << "null";
    return true;
  }

  if (value->IsScalar()) {
    int8_t char_value = 0;
    uint8_t uchar_value = 0;
    int16_t short_value = 0;
    uint16_t ushort_value = 0;
    int32_t int_value = 0;
    uint32_t uint_value = 0;
    int64_t long_value = 0;
    uint64_t ulong_value = 0;
    float float_value = 0;
    double double_value = 0;
    std::string string_value;

    if (CharValue::GetValue(value, &char_value)) {
      *result << static_cast<int>(char_value);
      return true;
    } else if (UCharValue::GetValue(value, &uchar_value)) {
      *result << static_cast<unsigned int>(uchar_value);
      return true;
    } else if (ShortValue::GetValue(value, &short_value)) {
      *result << short_value;
      return true;
    } else if (UShortValue::GetValue(value, &ushort_value)) {
      *result << ushort_value;
      return true;
    } else if (IntValue::GetValue(value, &int_value)) {
      *result << int_value;
      return true;
    } else if (UIntValue::GetValue(value, &uint_value)) {
      *result << uint_value;
      return true;
    } else if (LongValue::GetValue(value, &long_value)) {
      *result << long_value;
      return true;
    } else if (ULongValue::GetValue(value, &ulong_value)) {
      *result << ulong_value;
      return true;
    } else if (FloatValue::GetValue(value, &float_value)) {
      *result << float_value;
      return true;
    } else if (DoubleValue::GetValue(value, &double_value)) {
      *result << double_value;
      return true;
    } else if (value->AsString(&string_value)) {
      *result << "\"" << string_value << "\"";  // TODO(etienneb): escaping.
      return true;
    }

  } else if (value->IsAggregate()) {
    if (ArrayValueBase::InstanceOf(value)) {
      std::string indent_string = std::string(indent , ' ');
      std::string indent_field = std::string(indent + 4, ' ');
      const ArrayValueBase* array_value = ArrayValueBase::Cast(value);
      assert(array_value != nullptr);

      *result << "[\n";
      auto it = array_value->begin();
      for (; it != array_value->end(); ++it) {
        *result << indent_field;
        if (!ToString(&*it, indent + 4, result))
          return false;
        *result << "\n";
      }
      *result << indent_string << "]";
      return true;
    } else if (StructValueBase::InstanceOf(value)) {
      std::string indent_string = std::string(indent , ' ');
      std::string indent_field = std::string(indent + 4, ' ');
      const StructValueBase* struct_value = StructValueBase::Cast(value);
      assert(struct_value != nullptr);

      *result << "{\n";
      auto it = struct_value->fields_begin();
      for (; it != struct_value->fields_end(); ++it) {
        *result << indent_field << it->first << " = ";
        if (!ToString(it->second, indent + 4, result))
          return false;
        *result << "\n";
      }

      *result << indent_string << "}";
      return true;
    }
  }

  return false;
}

}  // namespace

bool ToString(const Value* value, std::string* result) {
  assert(result != nullptr);

  std::stringstream ss;
  if (!ToString(value, 0, &ss))
    return false;

  *result = ss.str();

  return true;
}

std::string ToString(const Value* value)
{
  std::string result;
  ToString(value, &result);
  return result;
}

}  // namespace value
}  // namespace tibee
