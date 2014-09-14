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

#include <cppunit/extensions/HelperMacros.h>
#include <vector>

#include <common/pipeline/MakeBackInserterSink.hpp>
#include <common/pipeline/MultiSink.hpp>

using namespace tibee::common;

class MultiSinkTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MultiSinkTest);
        CPPUNIT_TEST(testMultiSink);
    CPPUNIT_TEST_SUITE_END();

public:
    void testMultiSink();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MultiSinkTest);

void MultiSinkTest::testMultiSink()
{
    std::vector<int> actual_a;
    std::vector<int> actual_b;
    std::vector<int> expected;

    auto sink_a = MakeBackInserterSink(&actual_a);
    auto sink_b = MakeBackInserterSink(&actual_b);
    MultiSink<int> multi_sink;
    multi_sink.AddSink(&sink_a);
    multi_sink.AddSink(&sink_b);

    for (int i = 0; i < 10; ++i) {
        multi_sink.Receive(i);
        expected.push_back(i);
    }

    CPPUNIT_ASSERT(expected == actual_a);
    CPPUNIT_ASSERT(expected == actual_b);
}
