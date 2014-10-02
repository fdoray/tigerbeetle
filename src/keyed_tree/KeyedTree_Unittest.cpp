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
#include <set>
#include <string>

#include "gtest/gtest.h"
#include "keyed_tree/KeyedTree.hpp"
#include "quark/QuarkDatabase.hpp"

namespace tibee
{
namespace keyed_tree
{

TEST(KeyedTree, CreateNodeKey)
{
    KeyedTree<quark::Quark> tree;
    quark::QuarkDatabase<std::string> quarks;

    NodeKey a_b_1 = tree.CreateNodeKey({quarks.Insert("a"), quarks.Insert("b")});
    NodeKey a = tree.CreateNodeKey({quarks.Insert("a")});
    NodeKey a_b_2 = tree.CreateNodeKey(a, {quarks.Insert("b")});

    EXPECT_EQ(a_b_1.get(), a_b_2.get());

    NodeKey a_b_c_d_e_1 =
        tree.CreateNodeKey(a,
                         {quarks.Insert("b"),
                          quarks.Insert("c"),
                          quarks.Insert("d"),
                          quarks.Insert("e")});
    NodeKey a_b_c_d_e_2 =
        tree.CreateNodeKey(a_b_1,
                         {quarks.Insert("c"),
                          quarks.Insert("d"),
                          quarks.Insert("e")});

    EXPECT_EQ(a_b_c_d_e_1.get(), a_b_c_d_e_2.get());
}

TEST(KeyedTree, GetNodeKey)
{
    KeyedTree<quark::Quark> tree;
    quark::QuarkDatabase<std::string> quarks;

    NodeKey emptyExpected = tree.CreateNodeKey({});
    NodeKey emptyActual;
    EXPECT_TRUE(tree.GetNodeKey({}, &emptyActual));
    EXPECT_EQ(emptyExpected.get(), emptyActual.get());

    NodeKey abExpected = tree.CreateNodeKey({quarks.Insert("a"), quarks.Insert("b")});
    NodeKey abActual;
    EXPECT_TRUE(tree.GetNodeKey({quarks.Insert("a"), quarks.Insert("b")}, &abActual));
    EXPECT_EQ(abExpected.get(), abActual.get());

    NodeKey abcdExpected = tree.CreateNodeKey({quarks.Insert("a"), quarks.Insert("b"), quarks.Insert("c"), quarks.Insert("d")});
    NodeKey abcdActual;
    EXPECT_TRUE(tree.GetNodeKey({quarks.Insert("a"), quarks.Insert("b"), quarks.Insert("c"), quarks.Insert("d")},
                                &abcdActual));
    EXPECT_EQ(abcdExpected.get(), abcdActual.get());

    NodeKey aExpected = tree.CreateNodeKey({quarks.Insert("a")});
    NodeKey aActual;
    EXPECT_TRUE(tree.GetNodeKey({quarks.Insert("a")}, &aActual));
    EXPECT_EQ(aActual.get(), aExpected.get());

    EXPECT_FALSE(tree.GetNodeKey({quarks.Insert("b")}, &aActual));
    EXPECT_FALSE(tree.GetNodeKey({quarks.Insert("a"), quarks.Insert("c")}, &aActual));
}

TEST(KeyedTree, GetNodePath)
{
    KeyedTree<quark::Quark> tree;
    quark::QuarkDatabase<std::string> quarks;

    KeyedTree<quark::Quark>::Path expectedPathAbcd = {
      quarks.Insert("a"), quarks.Insert("b"), quarks.Insert("c"), quarks.Insert("d")
    };
    NodeKey keyAbcd = tree.CreateNodeKey(expectedPathAbcd);
    KeyedTree<quark::Quark>::Path actualPathAbcd;
    tree.GetNodePath(keyAbcd, &actualPathAbcd);
    EXPECT_EQ(expectedPathAbcd, actualPathAbcd);

    KeyedTree<quark::Quark>::Path expectedPathA = {
      quarks.Insert("a")
    };
    NodeKey keyA = tree.CreateNodeKey(expectedPathA);
    KeyedTree<quark::Quark>::Path actualPathA;
    tree.GetNodePath(keyA, &actualPathA);
    EXPECT_EQ(expectedPathA, actualPathA);

    KeyedTree<quark::Quark>::Path expectedPathEmpty = {};
    NodeKey keyEmpty = tree.CreateNodeKey(expectedPathEmpty);
    KeyedTree<quark::Quark>::Path actualPathEmpty;
    tree.GetNodePath(keyEmpty, &actualPathEmpty);
    EXPECT_EQ(expectedPathEmpty, actualPathEmpty);
}

TEST(KeyedTree, Iterator)
{
    KeyedTree<quark::Quark> tree;
    quark::QuarkDatabase<std::string> quarks;

    NodeKey a = tree.CreateNodeKey({quarks.Insert("a")});
    NodeKey a_b = tree.CreateNodeKey({quarks.Insert("a"), quarks.Insert("b")});
    NodeKey a_c = tree.CreateNodeKey({quarks.Insert("a"), quarks.Insert("c")});
    NodeKey a_d = tree.CreateNodeKey({quarks.Insert("a"), quarks.Insert("d")});

    auto it = tree.node_children_begin(a);
    auto it_end = tree.node_children_end(a);

    std::set<quark::Quark> children_quarks;
    std::set<size_t> children_keys;

    for (; it != it_end; ++it)
    {
      children_quarks.insert(it->first);
      children_keys.insert(it->second.get());
    }

    std::set<quark::Quark> expected_children_quarks {
        quarks.Insert("b"),
        quarks.Insert("c"),
        quarks.Insert("d")
    };
    std::set<size_t> expected_children_keys {
        a_b.get(), a_c.get(), a_d.get()};

    EXPECT_EQ(expected_children_quarks, children_quarks);
    EXPECT_EQ(expected_children_keys, children_keys);
}

}  // namespace state
}  // namespace tibee
