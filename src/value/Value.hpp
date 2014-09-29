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
//
// This file specifies a recursive data storage class called Value intended for
// storing event fields as a tree. Values are divided into two categories:
// scalar and aggregate. A scalar holds a value of a given basic type and an
// aggregate is a container for multiple disparate values.
//
// Usage examples:
// - Creation
//   std::unique_ptr<ArrayValue> my_array(new ArrayValue());
//   my_array->Append<IntValue>(42);
//   my_array->Append<IntValue>(1024);
//   my_array->Append<StringValue>("end");
//
//   std::unique_ptr<StructValue> top_struct(new StructValue());
//   top_struct->AddField("name1", std::move(my_array));
//   top_struct->AddField<LongValue>("name2", new LongValue(4U));
//
// - Introspection and casting
//   std::unique_ptr<IntValue> value(new IntValue(42));
//   if (value->IsInteger())
//    // value is an integer.
//
//   if (IntValue::InstanceOf(value.get())
//    const IntValue* int_value = IntValue::Cast(value.get());
//    // Use int_value to access IntValue specific methods.
//
// - Value accessor
//   std::unique_ptr<IntValue> value(new IntValue(42));
//
//   uint32_t result = 0;
//   if (value->GetAsUInteger(&result))
//     // do something with result
//
//   int32_t result = IntValue::GetValue(result.get());
//   // do something with result

#ifndef _TIBEE_VALUE_VALUE_HPP
#define _TIBEE_VALUE_VALUE_HPP

#include <assert.h>
#include <cstdlib>
#include <boost/utility.hpp>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace tibee {
namespace value {

enum ValueType {
  VALUE_BOOL,
  VALUE_CHAR,
  VALUE_UCHAR,
  VALUE_SHORT,
  VALUE_USHORT,
  VALUE_INT,
  VALUE_UINT,
  VALUE_LONG,
  VALUE_ULONG,
  VALUE_FLOAT,
  VALUE_DOUBLE,
  VALUE_STRING,
  VALUE_WSTRING,
  VALUE_STRUCT,
  VALUE_ARRAY
};

// The Value class is the base class for Values. Types are implemented by
// subclasses of Value. A cast from Value* to Subclass* is needed to access
// specific methods and fields. Some convenience methods ease the access to
// common functionalities.
class Value {
 public:
  typedef std::unique_ptr<Value> UP;

  // Destructor.
  virtual ~Value() { }

  // Returns the type of the value stored by the current Value object.
  virtual ValueType GetType() const = 0;

  // These methods return some properties of the value type.
  // @{
  virtual bool IsScalar() const = 0;
  virtual bool IsAggregate() const = 0;
  virtual bool IsInteger() const = 0;
  virtual bool IsSigned() const = 0;
  virtual bool IsFloating() const = 0;
  // @}

  // These methods allow the convenient retrieval of a basic value.
  // If the current value can be converted into the given type,
  // the value is returned through the |value| parameter.
  // @param value receives the value holded in this wrapper.
  // @returns true when the conversion is valid, false otherwise and |value|
  // stays unchanged.
  // @{
  bool GetAsInteger(int32_t* value) const;
  bool GetAsUInteger(uint32_t* value) const;
  bool GetAsLong(int64_t* value) const;
  bool GetAsULong(uint64_t* value) const;
  bool GetAsFloating(double* value) const;
  bool GetAsString(std::string* value) const;
  bool GetAsWString(std::wstring* value) const;
  // @}

  // Compare this value with the given value |value|.
  // @param value the value to compare with.
  // @returns true when both values are equal, false otherwise.
  virtual bool Equals(const Value* value) const = 0;

  // Check whether the dictionary has a value for the given field name.
  // @param name the name to check existence.
  // @returns true if the dictionary has a field named |name|.
  //     Always false if the value is not a dictionary.
  virtual bool HasField(const std::string& name) const { return false; }

  // Retrieve the value for a given name.
  // @param name the name of the field to find.
  // @returns the value of the field if the field is found, nullptr otherwise.
  //     Always nullptr if the value is not a dictionary.
  virtual const Value* GetField(const std::string& name) const { return nullptr; }

  // Retrieve the value for a given name.
  // @param name the name of the field to find.
  // @param value receives the value of the field with name |name|.
  // @returns true if the field is found, false otherwise.
  //     Always false if the value is not a dictionary.
  bool GetField(const std::string& name, const Value** value) const;

  // Retrieve the value of a given type for a given name.
  // @tparam T the type to cast the field value.
  // @param name the name of the field to find.
  // @param value receives the value of the field with name |name|.
  // @returns true if the field is found and of the specified type, false
  //     otherwise.
  //     Always false if the value is not a dictionary.
  template<class T>
  bool GetFieldAs(const std::string& name, const T** value) const {
    assert(value != nullptr);
    const Value* field = nullptr;
    if (!GetField(name, &field) || !T::InstanceOf(field))
      return false;
    *value = T::Cast(field);
    return true;
  }
};

template<class T, int TYPE>
class ScalarValue : public Value {
 public:
  typedef ScalarValue<T, TYPE> SelfType;
  typedef T ScalarType;

  // Overridden from Value:
  // @{
  virtual ValueType GetType() const override;
  virtual bool IsScalar() const override;
  virtual bool IsAggregate() const override;
  virtual bool IsInteger() const override;
  virtual bool IsSigned() const override;
  virtual bool IsFloating() const override;

  virtual bool Equals(const Value* value) const override;
  // @}

  // Retrieve the value holded in this wrapper.
  virtual const T& GetValue() const = 0;

  // Cast and retrieve the value holded in this wrapper.
  // @param value the value to retrieve (must be of the appropriate type).
  // @returns the value holded in this wrapper.
  static const T& GetValue(const Value* value);

  // Try to retrieve the value holded in |value|.
  // @param value the value to retrieve.
  // @param dst receives the value holded in this wrapper.
  // @returns true is the conversion is valid, false otherwise.
  static bool GetValue(const Value* value, T* dst);

  // Determine if |value| is of type |TYPE|.
  // @returns true is |value| has the appropriate type, false otherwise.
  static bool InstanceOf(const Value* value);

  // Cast |value| to type |TYPE|.
  // @param value the value to cast.
  // @returns the casted value.
  static const SelfType* Cast(const Value* value);

  // Returns the minimun value representable by |T|.
  static T MinValue();

  // Returns the maximal value representable by |T|.
  static T MaxValue();
};

typedef ScalarValue<bool, VALUE_BOOL> BoolValueBase;
typedef ScalarValue<int8_t, VALUE_CHAR> CharValueBase;
typedef ScalarValue<uint8_t, VALUE_UCHAR> UCharValueBase;
typedef ScalarValue<int16_t, VALUE_SHORT> ShortValueBase;
typedef ScalarValue<uint16_t, VALUE_USHORT> UShortValueBase;
typedef ScalarValue<int32_t, VALUE_INT> IntValueBase;
typedef ScalarValue<uint32_t, VALUE_UINT> UIntValueBase;
typedef ScalarValue<int64_t, VALUE_LONG> LongValueBase;
typedef ScalarValue<uint64_t, VALUE_ULONG> ULongValueBase;
typedef ScalarValue<std::string, VALUE_STRING> StringValueBase;
typedef ScalarValue<std::wstring, VALUE_WSTRING> WStringValueBase;
typedef ScalarValue<float, VALUE_FLOAT> FloatValueBase;
typedef ScalarValue<double, VALUE_DOUBLE> DoubleValueBase;

#define SIMPLE_VALUE_DEFINITION(TYPE) \
class TYPE : public TYPE##Base { \
 public: \
  TYPE () {} \
  explicit TYPE (const TYPE##Base::ScalarType& value) \
      : value_(value) { \
  } \
  virtual const TYPE##Base::ScalarType& GetValue() const override { \
    return value_; \
  } \
  void SetValue(const TYPE##Base::ScalarType& value) { \
    value_ = value; \
  } \
 private: \
  TYPE##Base::ScalarType value_; \
};

SIMPLE_VALUE_DEFINITION(BoolValue)
SIMPLE_VALUE_DEFINITION(CharValue)
SIMPLE_VALUE_DEFINITION(UCharValue)
SIMPLE_VALUE_DEFINITION(ShortValue)
SIMPLE_VALUE_DEFINITION(UShortValue)
SIMPLE_VALUE_DEFINITION(IntValue)
SIMPLE_VALUE_DEFINITION(UIntValue)
SIMPLE_VALUE_DEFINITION(LongValue)
SIMPLE_VALUE_DEFINITION(ULongValue)
SIMPLE_VALUE_DEFINITION(StringValue)
SIMPLE_VALUE_DEFINITION(WStringValue)
SIMPLE_VALUE_DEFINITION(FloatValue)
SIMPLE_VALUE_DEFINITION(DoubleValue)

template<int TYPE>
class AggregateValue : public Value {
 public:
  // Overridden from Value:
  // @{
  virtual ValueType GetType() const override;
  virtual bool IsScalar() const override;
  virtual bool IsAggregate() const override;
  virtual bool IsInteger() const override;
  virtual bool IsSigned() const override;
  virtual bool IsFloating() const override;
  // @}
};

// An ArrayValue holds a sequence of disparate values.
class ArrayValueBase
    : public AggregateValue<VALUE_ARRAY>,
      boost::noncopyable {
 public:
  ArrayValueBase();
  virtual ~ArrayValueBase();
  
  // Returns whether the array is empty.
  bool IsEmpty() const;

  // Returns the number of elements in the array.
  virtual size_t Length() const = 0;

  // Returns the element at position |index|.
  // @param index the offset of the element to retrieve.
  // @{
  const Value* operator[](size_t index) const;
  virtual const Value* at(size_t index) const = 0;
  // @}

  // Retrieve the value of a given type for the element at position |index|.
  // @tparam T the type to cast the element value.
  // @param index the offset of the element in the array.
  // @param value receives the value of the element.
  // @returns true if the element is found and of the specified type, false
  //     otherwise.
  template<class T>
  bool GetElementAs(size_t index, const T** value) const {
    assert(value != nullptr);
    if (index >= Length())
      return false;
    const Value* field = at(index);
    if (!T::InstanceOf(field))
      return false;
    *value = T::Cast(field);
    return true;
  }

  // These methods allow the convenient retrieval of an element of the array
  // with a basic value. If the current value can be converted into the given
  // type, the value is returned through the |value| parameter.
  // @param index the offset of the element in the array.
  // @param value receives the value holded by the field.
  // @returns true when the conversion is valid, false otherwise and |value|
  // stay unchanged.
  // @{
  bool GetElementAsInteger(size_t index, int32_t* value) const;
  bool GetElementAsUInteger(size_t index, uint32_t* value) const;
  bool GetElementAsLong(size_t index, int64_t* value) const;
  bool GetElementAsULong(size_t index, uint64_t* value) const;
  bool GetElementAsFloating(size_t index, double* value) const;
  bool GetElementAsString(size_t index, std::string* value) const;
  bool GetElementAsWString(size_t index, std::wstring* value) const;
  // @}

  // Overridden from Value:
  // @{
  virtual bool Equals(const Value* value) const override;
  // @}

  // Determine if |value| is of type ArrayType.
  // @param value the value to check type.
  // @returns true is |value| has the appropriate type, false otherwise.
  static bool InstanceOf(const Value* value);

  // Cast |value| to type ArrayType.
  // @param value the value to cast.
  // @returns the casted value.
  static const ArrayValueBase* Cast(const Value* value);

  // Interface of an array iterator implementation.
  class IteratorImpl :
      public std::iterator<std::input_iterator_tag, Value>
  {
  public:
    virtual IteratorImpl& operator++() = 0;
    virtual bool operator==(const IteratorImpl& other) const = 0;
    virtual bool operator!=(const IteratorImpl& other) const = 0;
    virtual const Value& operator*() const = 0;
    virtual const Value* operator->() const = 0;
  };

  // Proxy for an array iterator.
  class Iterator :
      public std::iterator<std::input_iterator_tag, Value>
  {
  public:
    Iterator(IteratorImpl* impl);
    Iterator& operator++();
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
    const Value& operator*() const;
    const Value* operator->() const;

  private:
    std::unique_ptr<IteratorImpl> impl_;
  };

  // Iteration.
  // @{
  virtual Iterator begin() const = 0;
  virtual Iterator end() const = 0;
  // @}
};

class ArrayValue : public ArrayValueBase {
 public:
  typedef std::unique_ptr<ArrayValue> UP;
  typedef std::vector<Value*> Values;
  typedef Values::const_iterator const_iterator;

  ArrayValue();
  ~ArrayValue();

  using ArrayValueBase::operator[];
  using ArrayValueBase::at;

  // Overridden from ArrayValueBase:
  // @{
  virtual size_t Length() const override;
  virtual const Value* at(size_t index) const override;
  virtual ArrayValueBase::Iterator begin() const override;
  virtual ArrayValueBase::Iterator end() const override;
  // }@

  // Returns the non-const element at position |index|.
  // @param index the offset of the element to retrieve.
  // @{
  Value* operator[](size_t index);
  Value* at(size_t index);
  // }@

  // Appends a Value to the end of the sequence.
  // Take the ownership of |value|.
  // @param value the value to add.
  void Append(std::unique_ptr<Value> value);

  // Allocates and appends a typed value to the array.
  // @tparam T a scalar value type (i.e. CharValue, IntValue, ...).
  // @param value the value to add.
  template<class T>
  void Append(const typename T::ScalarType& value) {
    std::unique_ptr<Value> ptr(new T(value));
    Append(std::move(ptr));
  }

  // Appends to the end of the sequence a series of value.
  // @param value the values to add.
  // @param length the number of values to add.
  template<class T>
  void AppendAll(const typename T::ScalarType* value, size_t length) {
    for (size_t i = 0; i < length; ++i)
      Append<T>(value[i]);
  }

 private:
  // Implementation of an array iterator.
  class IteratorImpl :
      public ArrayValueBase::IteratorImpl {
   public:
    IteratorImpl(const_iterator it);

    virtual ArrayValueBase::IteratorImpl& operator++() override;
    virtual bool operator==(
        const ArrayValueBase::IteratorImpl& other) const override;
    virtual bool operator!=(
        const ArrayValueBase::IteratorImpl& other) const override;
    virtual const Value& operator*() const override;
    virtual const Value* operator->() const override;

  private:
    const_iterator it_;
  };

  Values values_;
};

// StructValue provides a key-value dictionary and keeps fields in a sequence.
class StructValueBase
    : public AggregateValue<VALUE_STRUCT>,
      boost::noncopyable {
 public:
  StructValueBase();
  virtual ~StructValueBase();

  // Returns the number of elements in the struct.
  virtual size_t Length() const = 0;

  using Value::GetField;

  // Overridden from value::Value:
  // @{
  virtual bool HasField(const std::string& name) const override = 0;
  virtual const Value* GetField(const std::string& name) const override = 0;
  // }@

  // Retrieve the value for a given index.
  // @param index the index of the field to find.
  // @returns the value of the field if the field is found, nullptr otherwise.
  const Value* operator[](size_t index) const;
  virtual const Value* at(size_t index) const = 0;

  // These methods allow the convenient retrieval of a field with a basic
  // value. If the current value can be converted into the given type,
  // the value is returned through the |value| parameter.
  // @param name the name of the field to retrieve.
  // @param value receives the value holded by the field.
  // @returns true when the conversion is valid, false otherwise and |value|
  // stay unchanged.
  // @{
  bool GetFieldAsInteger(const std::string& name, int32_t* value) const;
  bool GetFieldAsUInteger(const std::string& name, uint32_t* value) const;
  bool GetFieldAsLong(const std::string& name, int64_t* value) const;
  bool GetFieldAsULong(const std::string& name, uint64_t* value) const;
  bool GetFieldAsFloating(const std::string& name, double* value) const;
  bool GetFieldAsString(const std::string& name, std::string* value) const;
  bool GetFieldAsWString(const std::string& name, std::wstring* value) const;
  // @}

  // Overridden from Value:
  // @{
  virtual bool Equals(const Value* value) const override;
  // @}

  // Determine if |value| is of type StructType.
  // @returns true is |value| has the appropriate type, false otherwise.
  static bool InstanceOf(const Value* value);

  // Cast |value| to type StructType.
  // @param value the value to cast.
  // @returns the casted value.
  static const StructValueBase* Cast(const Value* value);

  // Interface of a struct iterator implementation.
  class IteratorImpl :
      public std::iterator<std::input_iterator_tag,
                           std::pair<const std::string, Value*>>
  {
  public:
    virtual IteratorImpl& operator++() = 0;
    virtual bool operator==(const IteratorImpl& other) const = 0;
    virtual bool operator!=(const IteratorImpl& other) const = 0;
    virtual const std::pair<const std::string, const Value*>&
        operator*() const = 0;
    virtual const std::pair<const std::string, const Value*>*
        operator->() const = 0;
  };

  // Proxy for a struct iterator.
  class Iterator :
      public std::iterator<std::input_iterator_tag,
                           std::pair<const std::string, Value*>>
  {
  public:
    Iterator(IteratorImpl* impl);
    Iterator& operator++();
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
    const std::pair<const std::string, const Value*>&
        operator*() const;
    const std::pair<const std::string, const Value*>*
        operator->() const;

  private:
    std::unique_ptr<IteratorImpl> impl_;
  };

  // Iteration.
  // @{
  virtual Iterator fields_begin() const = 0;
  virtual Iterator fields_end() const = 0;
  // @}
};

class StructValue : public StructValueBase {
 public:
  typedef std::unique_ptr<StructValue> UP;
  typedef std::map<std::string, Value*> ValueMap;
  typedef std::vector<ValueMap::value_type*> ValueVector;
  typedef ValueVector::const_iterator const_iterator;
  
  StructValue();
  virtual ~StructValue();

  using StructValueBase::GetField;

  // Overridden from StructValueBase:
  // @{
  virtual size_t Length() const override;
  virtual bool HasField(const std::string& name) const override;
  virtual const Value* GetField(const std::string& name) const override;
  virtual const Value* at(size_t index) const override;
  virtual StructValueBase::Iterator fields_begin() const override;
  virtual StructValueBase::Iterator fields_end() const override;
  // @}

  // Add a field with name |name| to this structure.
  // @param name the name of the field.
  // @param value the value of the field.
  // @returns true if the field can be added, false otherwise.
  bool AddField(const std::string& name, std::unique_ptr<Value> value);

  // Add a field with name |name| to this structure.
  // @tparam T the type of the value of the field.
  // @param name the name of the field.
  // @param value the value of the field.
  // @returns true if the field can be added, false otherwise.
  template<class T>
  bool AddField(const std::string& name, const typename T::ScalarType& value) {
    std::unique_ptr<Value> ptr(new T(value));
    return AddField(name, std::move(ptr));
  }

 private:
  // Implementation of a struct iterator.
  class IteratorImpl :
      public StructValueBase::IteratorImpl {
   public:
    IteratorImpl(const_iterator it);

    virtual StructValueBase::IteratorImpl& operator++() override;
    virtual bool operator==(
        const StructValueBase::IteratorImpl& other) const override;
    virtual bool operator!=(
        const StructValueBase::IteratorImpl& other) const override;
    virtual const std::pair<const std::string, const Value*>&
        operator*() const override;
    virtual const std::pair<const std::string, const Value*>*
        operator->() const override;

  private:
    mutable std::unique_ptr<std::pair<const std::string, const Value*>>
        currentPair_;
    const_iterator it_;
  };

  ValueVector fields_;
  ValueMap fields_map_;
};

}  // namespace value
}  // namesace tibee

#endif  // _TIBEE_VALUE_VALUE_HPP
