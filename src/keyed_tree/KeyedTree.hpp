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
#ifndef _TIBEE_KEYEDTREE_KEYEDTREE_HPP
#define _TIBEE_KEYEDTREE_KEYEDTREE_HPP

#include <assert.h>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <utility>

#include "keyed_tree/NodeKey.hpp"

namespace tibee
{
namespace keyed_tree
{

/**
 * Keyed tree.
 *
 * A tree in which the children of each node are identified
 * by labels.
 *
 * @author Francois Doray
 */
template <typename T>
class KeyedTree
{
private:
    struct Node;
    typedef std::unordered_map<T, Node*> Nodes;

    struct Node
    {
        NodeKey key;
        Nodes children;
    };

public:
    typedef std::vector<T> Path;

    KeyedTree();
    ~KeyedTree();

    NodeKey GetNodeKey(const Path& path);
    NodeKey GetNodeKey(NodeKey root, const Path& subPath);

    typedef std::pair<T, NodeKey> LabelNodeKeyPair;
    class Iterator :
        public std::iterator<std::input_iterator_tag, LabelNodeKeyPair>
    {
    public:
        friend class KeyedTree<T>;

        Iterator();
        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        const LabelNodeKeyPair& operator*() const;
        const LabelNodeKeyPair* operator->() const;

    private:
        Iterator(typename KeyedTree<T>::Nodes::const_iterator it);

        mutable LabelNodeKeyPair _currentPair;
        typename KeyedTree<T>::Nodes::const_iterator _it;
    };

    Iterator node_children_begin(NodeKey key) const;
    Iterator node_children_end(NodeKey key) const;

private:
    Node* GetNode(Node* root, const Path& subPath);

    Node _root;

    typedef std::vector<std::unique_ptr<Node>> NodeVector;
    NodeVector _nodes;
};

template <typename T>
KeyedTree<T>::KeyedTree()
{
}

template <typename T>
KeyedTree<T>::~KeyedTree()
{
}

template <typename T>
NodeKey KeyedTree<T>::GetNodeKey(const Path& path)
{
    return GetNode(&_root, path)->key;
}

template <typename T>
NodeKey KeyedTree<T>::GetNodeKey(NodeKey root, const Path& subPath)
{
    Node* rootNode = _nodes[root.get()].get();
    return GetNode(rootNode, subPath)->key;
}

template <typename T>
typename KeyedTree<T>::Node* KeyedTree<T>::GetNode(Node* root, const Path& subPath)
{
    Node* currentNode = root;
    for (T label : subPath)
    {
        auto look = currentNode->children.find(label);
        if (look == currentNode->children.end())
        {
            std::unique_ptr<Node> newNode { new Node };
            auto newNodePtr = newNode.get();
            newNodePtr->key = NodeKey(_nodes.size());

            _nodes.push_back(std::move(newNode));

            currentNode->children[label] = newNodePtr;
            currentNode = newNodePtr;
        }
        else
        {
            currentNode = look->second;
        }
    }

    return currentNode;
}

template <typename T>
typename KeyedTree<T>::Iterator KeyedTree<T>::node_children_begin(NodeKey key) const
{
    assert(key.get() < _nodes.size());
    const Node& node = *_nodes[key.get()];
    return Iterator(node.children.begin());
}

template <typename T>
typename KeyedTree<T>::Iterator KeyedTree<T>::node_children_end(NodeKey key) const
{
    assert(key.get() < _nodes.size());
    const Node& node = *_nodes[key.get()];
    return Iterator(node.children.end());
}

template <typename T>
KeyedTree<T>::Iterator::Iterator() {}

template <typename T>
KeyedTree<T>::Iterator::Iterator(typename KeyedTree<T>::Nodes::const_iterator it) :
    _it(it)
{
}

template <typename T>
typename KeyedTree<T>::Iterator& KeyedTree<T>::Iterator::operator++()
{
    ++_it;
    return *this;
}

template <typename T>
bool KeyedTree<T>::Iterator::operator==(const Iterator& other) const
{
    return _it == other._it;
}

template <typename T>
bool KeyedTree<T>::Iterator::operator!=(const Iterator& other) const
{
    return _it != other._it;
}

template <typename T>
const typename KeyedTree<T>::LabelNodeKeyPair& KeyedTree<T>::Iterator::operator*() const
{
    _currentPair.first = _it->first;
    _currentPair.second = _it->second->key;
    return _currentPair;
}

template <typename T>
const typename KeyedTree<T>::LabelNodeKeyPair* KeyedTree<T>::Iterator::operator->() const
{
    return &(**this);
}

}
}

#endif // _TIBEE_KEYEDTREE_KEYEDTREE_HPP
