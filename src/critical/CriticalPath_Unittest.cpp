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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.    If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>

#include "gtest/gtest.h"
#include "critical/CriticalPath.hpp"
#include "critical/CriticalTypes.hpp"

namespace tibee {
namespace critical {

TEST(CriticalPath, CriticalPath)
{
    CriticalPath path;
    path.Push(CriticalPathSegment(0, 1, CriticalEdgeType::kRunUsermode));
    path.Push(CriticalPathSegment(1, 2, CriticalEdgeType::kRunUsermode));
    path.Push(CriticalPathSegment(2, 3, CriticalEdgeType::kRunUsermode));
    path.Push(CriticalPathSegment(3, 2, CriticalEdgeType::kRunUsermode));
    path.Push(CriticalPathSegment(4, 1, CriticalEdgeType::kRunUsermode));
    path.RestrictToThreads({1, 2});

    auto it = path.begin();
    auto it_end = path.end();

    ASSERT_NE(it, it_end);
    EXPECT_EQ(0, it->ts);
    EXPECT_EQ(1, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(1, it->ts);
    EXPECT_EQ(2, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(2, it->ts);
    EXPECT_EQ(2, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(3, it->ts);
    EXPECT_EQ(2, it->tid);
    ++it;

    ASSERT_NE(it, it_end);
    EXPECT_EQ(4, it->ts);
    EXPECT_EQ(1, it->tid);
    ++it;

    EXPECT_EQ(it, it_end);
}


}    // namespace critical
}    // namespace tibee
