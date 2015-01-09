/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tibeecompare.
 *
 * tibeecompare is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tibeecompare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tibeecompare.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "gtest/gtest.h"

#include "quark/DiskQuarkDatabase.hpp"

namespace tibee
{
namespace quark
{

TEST(DiskQuarkDatabase, DiskQuarkDatabase)
{
    const char kFilename[] = "test-db.tmp";

    DiskQuarkDatabase::UP db(new DiskQuarkDatabase(kFilename));
    auto q1 = db->StrQuark("one");
    auto q2 = db->StrQuark("two");
    auto q3 = db->StrQuark("three");

    EXPECT_EQ("one", db->String(q1));
    EXPECT_EQ("two", db->String(q2));
    EXPECT_EQ("three", db->String(q3));

    db.reset(nullptr);
    db.reset(new DiskQuarkDatabase(kFilename));

    auto q4 = db->StrQuark("four");
    auto q5 = db->StrQuark("five");
    auto q6 = db->StrQuark("six");

    EXPECT_EQ("one", db->String(q1));
    EXPECT_EQ("two", db->String(q2));
    EXPECT_EQ("three", db->String(q3));
    EXPECT_EQ("four", db->String(q4));
    EXPECT_EQ("five", db->String(q5));
    EXPECT_EQ("six", db->String(q6));

    EXPECT_EQ("1", db->String(db->IntQuark(1)));
    EXPECT_EQ("2", db->String(db->IntQuark(2)));
    EXPECT_EQ(db->IntQuark(1), db->StrQuark("1"));
    EXPECT_EQ(db->IntQuark(2), db->StrQuark("2"));
}

}  // namespace quark
}  // namespace tibee
