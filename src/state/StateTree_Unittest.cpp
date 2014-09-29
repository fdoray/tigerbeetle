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

}  // namespace state
}  // namespace tibee
