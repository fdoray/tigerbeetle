/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "state/CurrentState.hpp"

#include <assert.h>

namespace tibee
{
namespace state
{

namespace
{
const int kMaxIntQuark = 65535;
}  // namespace

CurrentState::CurrentState(OnAttributeChangeCallback onAttributeChangeCallback) :
    _ts(0),
    _onAttributeChangeCallback(onAttributeChangeCallback)
{
    _intQuarks.reserve(kMaxIntQuark + 1);
    for (int i = 0; i <= kMaxIntQuark; ++i)
        _intQuarks.push_back(Quark(std::to_string(i)));
}

CurrentState::~CurrentState()
{
}

quark::Quark CurrentState::IntQuark(int val)
{
    if (val >= 0 && val <= kMaxIntQuark)
        return _intQuarks[val];
    return Quark(std::to_string(val));
}

quark::Quark CurrentState::Quark(const std::string& str)
{
    quark::Quark quark = _quarks.Insert(str);
    return quark;
}

const std::string& CurrentState::String(quark::Quark quark) const
{
    return _quarks.ValueOf(quark);
}

AttributeKey CurrentState::GetAttributeKey(const AttributePath& path)
{
    return _attributeTree.CreateNodeKey(path);
}

AttributeKey CurrentState::GetAttributeKeyStr(const AttributePathStr& pathStr)
{
    AttributePath path;
    path.reserve(pathStr.size());
    for (const auto& str : pathStr)
        path.push_back(Quark(str));
    return GetAttributeKey(path);
}

AttributeKey CurrentState::GetAttributeKey(AttributeKey root, const AttributePath& subPath)
{
    return _attributeTree.CreateNodeKey(root, subPath);
}

void CurrentState::SetAttribute(AttributeKey attribute, value::Value::UP value)
{
    if (_onAttributeChangeCallback != nullptr)
        _onAttributeChangeCallback(attribute, value.get());

    AttributeValue& attributeValue = _attributeValues[attribute.get()];
    attributeValue.value = std::move(value);
    attributeValue.since = _ts;
}

void CurrentState::SetAttribute(AttributeKey attribute, const AttributePath& subPath, value::Value::UP value)
{
    AttributeKey subPathKey = GetAttributeKey(attribute, subPath);
    SetAttribute(subPathKey, std::move(value));
}

void CurrentState::SetAttribute(const AttributePath& path, value::Value::UP value)
{
    AttributeKey key = GetAttributeKey(path);
    SetAttribute(key, std::move(value));
}

void CurrentState::NullAttribute(AttributeKey attribute)
{
    SetAttribute(attribute, value::Value::UP {});

    auto it = _attributeTree.node_children_begin(attribute);
    auto it_end = _attributeTree.node_children_end(attribute);
    for (; it != it_end; ++it)
        NullAttribute(it->second);
}

void CurrentState::NullAttribute(AttributeKey attribute, const AttributePath& subPath)
{
    AttributeKey subPathKey = GetAttributeKey(attribute, subPath);
    NullAttribute(subPathKey);
}

void CurrentState::NullAttribute(const AttributePath& path)
{
    AttributeKey key = GetAttributeKey(path);
    NullAttribute(key);
}

const value::Value* CurrentState::GetAttributeValue(AttributeKey attribute)
{
    AttributeValue& attributeValue = _attributeValues[attribute.get()];
    return attributeValue.value.get();
}

const value::Value* CurrentState::GetAttributeValue(AttributeKey attribute, const AttributePath& subPath)
{
    AttributeKey subPathKey = GetAttributeKey(attribute, subPath);
    return GetAttributeValue(subPathKey);
}

const value::Value* CurrentState::GetAttributeValue(const AttributePath& path)
{
    AttributeKey key = GetAttributeKey(path);
    return GetAttributeValue(key);
}

timestamp_t CurrentState::GetAttributeLastChange(AttributeKey attribute)
{
    AttributeValue& attributeValue = _attributeValues[attribute.get()];
    return attributeValue.since;
}

timestamp_t CurrentState::GetAttributeLastChange(AttributeKey attribute, const AttributePath& subPath)
{
    AttributeKey subPathKey = GetAttributeKey(attribute, subPath);
    return GetAttributeLastChange(subPathKey);
}

timestamp_t CurrentState::GetAttributeLastChange(const AttributePath& path)
{
    AttributeKey key = GetAttributeKey(path);
    return GetAttributeLastChange(key);
}

void CurrentState::GetAttributePath(AttributeKey attribute, AttributeTree::Path* path) const
{
    _attributeTree.GetNodePath(attribute, path);
}

CurrentState::AttributeValue::AttributeValue() :
    since(0)
{
}

}
}
