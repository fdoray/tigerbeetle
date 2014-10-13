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

#ifndef _TIBEE_VALUE_MAKEVALUE_HPP
#define _TIBEE_VALUE_MAKEVALUE_HPP

#include <memory>
#include <string>

#include "quark/Quark.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace value
{

inline std::unique_ptr<Value> MakeValue(quark::Quark value)
{
    std::unique_ptr<Value> value_wrapper { new UIntValue { value.get() } };
    return value_wrapper;
}

inline std::unique_ptr<Value> MakeValue(int32_t value)
{
    std::unique_ptr<Value> value_wrapper { new IntValue { value } };
    return value_wrapper;
}

inline std::unique_ptr<Value> MakeValue(uint32_t value)
{
    std::unique_ptr<Value> value_wrapper { new UIntValue { value } };
    return value_wrapper;
}

inline std::unique_ptr<Value> MakeValue(int64_t value)
{
    std::unique_ptr<Value> value_wrapper { new LongValue { value } };
    return value_wrapper;
}

inline std::unique_ptr<Value> MakeValue(uint64_t value)
{
    std::unique_ptr<Value> value_wrapper { new ULongValue { value } };
    return value_wrapper;
}

inline std::unique_ptr<Value> MakeValue(const std::string& value)
{
    std::unique_ptr<Value> value_wrapper { new StringValue { value } };
    return value_wrapper;
}

inline std::unique_ptr<Value> MakeNullValue()
{
    std::unique_ptr<Value> value_wrapper { };
    return value_wrapper;
}

}  // namespace value
}  // namespace tibee

#endif  // _TIBEE_VALUE_MAKEVALUE_HPP