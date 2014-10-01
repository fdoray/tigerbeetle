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
#ifndef _TIBEE_STATE_STATETREE_HPP
#define _TIBEE_STATE_STATETREE_HPP

#include <iterator>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "quark/Quark.hpp"
#include "state/AttributeKey.hpp"
#include "state/AttributePath.hpp"

namespace tibee
{
namespace state
{

/**
 * Attribute tree.
 *
 * @author Francois Doray
 */
class AttributeTree
{
public:
    struct Attribute;
    typedef std::unordered_map<quark::Quark, Attribute*> Attributes;
    struct Attribute
    {
        AttributeKey key;
        Attributes children;
    };

    AttributeTree();
    ~AttributeTree();

    AttributeKey GetAttributeKey(const AttributePath& path);
    AttributeKey GetAttributeKey(AttributeKey root, const AttributePath& subPath);

    typedef std::pair<quark::Quark, AttributeKey> QuarkAttributeKeyPair;
    class Iterator :
        public std::iterator<std::input_iterator_tag, QuarkAttributeKeyPair>
    {
    public:
        friend class AttributeTree;

        Iterator();
        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        const QuarkAttributeKeyPair& operator*() const;
        const QuarkAttributeKeyPair* operator->() const;

    private:
        Iterator(AttributeTree::Attributes::const_iterator it);

        mutable QuarkAttributeKeyPair _currentPair;
        AttributeTree::Attributes::const_iterator _it;
    };

    Iterator attribute_children_begin(AttributeKey key) const;
    Iterator attribute_children_end(AttributeKey key) const;

private:
    Attribute* GetAttribute(Attribute* root, const AttributePath& subPath);

    Attribute _root;

    typedef std::vector<std::unique_ptr<Attribute>> AttributeVector;
    AttributeVector _attributes;
};

}
}

#endif // _TIBEE_STATE_STATETREE_HPP
