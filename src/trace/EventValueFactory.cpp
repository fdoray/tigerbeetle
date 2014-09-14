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
#include <memory>
#include <functional>
#include <babeltrace/ctf/events.h>
#include <iostream>
#include "trace/EventValueFactory.hpp"

namespace tibee
{
namespace trace
{

EventValueFactory::EventValueFactory() :
    _arrayPool {128},
    _structPool {32},
    _enumPool {64},
    _doublePool {64},
    _longPool {128},
    _stringPool {64},
    _ulongPool {128}
{
    // initialize types
    this->initTypes();
}

EventValueFactory::~EventValueFactory()
{
}

void EventValueFactory::initTypes()
{
    // precious builder functions
    auto unknownBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev)
    {
        return nullptr;
    };

    auto longBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev) -> const value::Value*
    {
        auto decl = ::bt_ctf_get_decl_from_def(def);

        if (::bt_ctf_get_int_signedness(decl) == 1) {
            return new(_longPool.get()) value::LongValue {::bt_ctf_get_int64(def)};
        } else {
            return new(_ulongPool.get()) value::ULongValue {::bt_ctf_get_uint64(def)};
        }
    };

    auto doubleBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev)
    {
        return new(_doublePool.get()) value::DoubleValue {::bt_ctf_get_float(def)};
    };

    auto enumBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev)
    {
        auto intDef = ::bt_ctf_get_enum_int(def);
        return new(_enumPool.get()) value::ULongValue {::bt_ctf_get_uint64(intDef)};
    };

    auto stringBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev)
    {
        return new(_stringPool.get()) value::StringValue {::bt_ctf_get_string(def)};
    };

    auto structBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev)
    {
        return new(_structPool.get()) StructEventValue {def, ev, this};
    };

    auto variantBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev)
    {
        auto realDef = ::bt_ctf_get_variant(def);
        return this->buildEventValue(realDef, ev);
    };

    auto arraySequenceBuilder = [this] (const ::bt_definition* def, const ::bt_ctf_event* ev) -> const value::Value*
    {
        const ::bt_declaration* decl = ::bt_ctf_get_decl_from_def(def);
        auto encoding = ::bt_ctf_get_encoding(decl);
        if (encoding == ::CTF_STRING_UTF8 || encoding == ::CTF_STRING_ASCII) {
            if (::bt_ctf_field_type(decl) == CTF_TYPE_SEQUENCE) {
                // TODO(fdoray): Find a way to retrieve a CTF sequence string.
                return new(_stringPool.get()) value::StringValue {"/ Unable to read ctf string sequence. /"};
            } else {
                auto str = ::bt_ctf_get_char_array(def);
                return new(_stringPool.get()) value::StringValue {str};
            }
        }
        return new(_arrayPool.get()) ArrayEventValue {def, ev, this};
    };

    // fill our builders
    for (auto& builder : _builders) {
        builder = unknownBuilder;
    }

    _builders[::CTF_TYPE_INTEGER] = longBuilder;
    _builders[::CTF_TYPE_FLOAT] = doubleBuilder;
    _builders[::CTF_TYPE_ENUM] = enumBuilder;
    _builders[::CTF_TYPE_STRING] = stringBuilder;
    _builders[::CTF_TYPE_STRUCT] = structBuilder;
    _builders[::CTF_TYPE_VARIANT] = variantBuilder;
    _builders[::CTF_TYPE_ARRAY] = arraySequenceBuilder;
    _builders[::CTF_TYPE_SEQUENCE] = arraySequenceBuilder;
}

const value::Value* EventValueFactory::buildEventValue(const ::bt_definition* def,
                                                       const ::bt_ctf_event* ev) const
{
    // get event value type
    auto decl = ::bt_ctf_get_decl_from_def(def);
    auto valueType = ::bt_ctf_field_type(decl);

    // call builder
    return _builders[valueType](def, ev);
}

void EventValueFactory::resetPools()
{
    _arrayPool.reset();
    _structPool.reset();
    _enumPool.reset();
    _doublePool.reset();
    _longPool.reset();
    _stringPool.reset();
    _ulongPool.reset();
}

}
}
