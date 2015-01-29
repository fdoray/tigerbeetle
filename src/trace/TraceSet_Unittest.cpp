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
#include "gtest/gtest.h"
#include "trace/TraceSet.hpp"

namespace tibee {
namespace trace {

TEST(TraceSet, addTrace)
{
    TraceSet traceSet;
    EXPECT_TRUE(traceSet.addTrace("test_data/ust_a/ust/uid/1000/64-bit"));
}

TEST(TraceSet, getBegin)
{
    TraceSet traceSet;
    EXPECT_TRUE(traceSet.addTrace("test_data/ust_b/ust/uid/1000/64-bit"));
    EXPECT_EQ(1411853469186692760u, traceSet.getBegin());
    EXPECT_TRUE(traceSet.addTrace("test_data/ust_a/ust/uid/1000/64-bit"));
    EXPECT_EQ(1411853296688683178u, traceSet.getBegin());
}

TEST(TraceSet, getEnd)
{
    TraceSet traceSet;
    EXPECT_TRUE(traceSet.addTrace("test_data/ust_a/ust/uid/1000/64-bit"));
    EXPECT_EQ(1411853296690333095u, traceSet.getEnd());
    EXPECT_TRUE(traceSet.addTrace("test_data/ust_b/ust/uid/1000/64-bit"));
    EXPECT_EQ(1411853469196893568u, traceSet.getEnd());
}

}  // namespace value
}  // namespace tibee
