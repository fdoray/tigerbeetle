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
#include "base/Constants.hpp"
#include "state/CurrentState.hpp"
#include "value/Utils.hpp"

namespace tibee
{
namespace state
{

CurrentState::CurrentState(OnAttributeChangeCallback onAttributeChangeCallback,
                           quark::StringQuarkDatabase* quarks) :
    _ts(0),
    _quarks(quarks),
    _onAttributeChangeCallback(onAttributeChangeCallback)
{
    assert(_quarks != nullptr);

    Q_CUR_THREAD = Quark(kStateCurThread);
    _cpusAttribute = GetAttributeKey({Quark(kStateLinux), Quark(kStateCpus)});
}

CurrentState::~CurrentState()
{
}

quark::Quark CurrentState::IntQuark(int val)
{
    return _quarks->IntQuark(val);
}

quark::Quark CurrentState::Quark(const std::string& str)
{
    return _quarks->StrQuark(str);
}

const std::string& CurrentState::String(quark::Quark quark) const
{
    return _quarks->String(quark);
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
    if (value::Value::AreEqual(value.get(), GetAttributeValue(attribute)))
        return;

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

uint32_t CurrentState::CurrentThreadForCpu(uint32_t cpu)
{
    auto threadValue = GetAttributeValue(_cpusAttribute, {IntQuark(cpu), Q_CUR_THREAD});
    if (threadValue == nullptr)
        return kInvalidThread;
    auto tid = threadValue->AsInteger();
    if (tid == 0)
        return kInvalidThread;
    return tid;
}

CurrentState::AttributeValue::AttributeValue() :
    since(0)
{
}

}
}
