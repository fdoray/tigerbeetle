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
#include "quark/QuarkDatabase.hpp"
#include "state/StateTree.hpp"

namespace tibee
{
namespace state
{

TEST(StateTree, StateTree)
{
    StateTree tree;
    quark::QuarkDatabase<std::string> quarks;

    StateKey a_b_1 = tree.GetStateKey({quarks.Insert("a"), quarks.Insert("b")});
    StateKey a = tree.GetStateKey({quarks.Insert("a")});
    StateKey a_b_2 = tree.GetStateKey(a, {quarks.Insert("b")});

    EXPECT_EQ(a_b_1.get(), a_b_2.get());

    StateKey a_b_c_d_e_1 =
        tree.GetStateKey(a,
                         {quarks.Insert("b"),
                          quarks.Insert("c"),
                          quarks.Insert("d"),
                          quarks.Insert("e")});
    StateKey a_b_c_d_e_2 =
        tree.GetStateKey(a_b_1,
                         {quarks.Insert("c"),
                          quarks.Insert("d"),
                          quarks.Insert("e")});

    EXPECT_EQ(a_b_c_d_e_1.get(), a_b_c_d_e_2.get());
}

TEST(StateTree, Iterator)
{
    StateTree tree;
    quark::QuarkDatabase<std::string> quarks;

    StateKey a = tree.GetStateKey({quarks.Insert("a")});
    StateKey a_b = tree.GetStateKey({quarks.Insert("a"), quarks.Insert("b")});
    StateKey a_c = tree.GetStateKey({quarks.Insert("a"), quarks.Insert("c")});
    StateKey a_d = tree.GetStateKey({quarks.Insert("a"), quarks.Insert("d")});

    auto it = tree.state_children_begin(a);
    auto it_end = tree.state_children_end(a);

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
