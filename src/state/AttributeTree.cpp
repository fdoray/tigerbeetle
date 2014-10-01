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
#include "state/AttributeTree.hpp"

#include <assert.h>

namespace tibee
{
namespace state
{

AttributeTree::AttributeTree()
{
}

AttributeTree::~AttributeTree()
{
}

AttributeKey AttributeTree::GetAttributeKey(const AttributePath& path)
{
    return GetAttribute(&_root, path)->key;
}

AttributeKey AttributeTree::GetAttributeKey(AttributeKey root, const AttributePath& subPath)
{
    Attribute* rootAttribute = _attributes[root.get()].get();
    return GetAttribute(rootAttribute, subPath)->key;
}

AttributeTree::Attribute* AttributeTree::GetAttribute(Attribute* root, const AttributePath& subPath)
{
    Attribute* currentAttribute = root;
    for (quark::Quark quark : subPath)
    {
        auto look = currentAttribute->children.find(quark);
        if (look == currentAttribute->children.end())
        {
            std::unique_ptr<Attribute> newAttribute { new Attribute };
            auto newAttributePtr = newAttribute.get();
            newAttributePtr->key = AttributeKey(_attributes.size());

            _attributes.push_back(std::move(newAttribute));

            currentAttribute->children[quark] = newAttributePtr;
            currentAttribute = newAttributePtr;
        }
        else
        {
            currentAttribute = look->second;
        }
    }

    return currentAttribute;
}

AttributeTree::Iterator AttributeTree::attribute_children_begin(AttributeKey key) const
{
    assert(key.get() < _attributes.size());
    const Attribute& attribute = *_attributes[key.get()];
    return Iterator(attribute.children.begin());
}

AttributeTree::Iterator AttributeTree::attribute_children_end(AttributeKey key) const
{
    assert(key.get() < _attributes.size());
    const Attribute& attribute = *_attributes[key.get()];
    return Iterator(attribute.children.end());
}

AttributeTree::Iterator::Iterator() {}

AttributeTree::Iterator::Iterator(AttributeTree::Attributes::const_iterator it) :
    _it(it)
{
}

AttributeTree::Iterator& AttributeTree::Iterator::operator++()
{
    ++_it;
    return *this;
}

bool AttributeTree::Iterator::operator==(const Iterator& other) const
{
    return _it == other._it;
}

bool AttributeTree::Iterator::operator!=(const Iterator& other) const
{
    return _it != other._it;
}

const AttributeTree::QuarkAttributeKeyPair& AttributeTree::Iterator::operator*() const
{
    _currentPair.first = _it->first;
    _currentPair.second = _it->second->key;
    return _currentPair;
}

const AttributeTree::QuarkAttributeKeyPair* AttributeTree::Iterator::operator->() const
{
    return &(**this);
}


}
}
