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

#include <common/pipeline/CallbackSink.hpp>
#include <common/pipeline/MakeBackInserterSink.hpp>

using namespace tibee::common;

class MakeBackInserterSinkTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MakeBackInserterSinkTest);
        CPPUNIT_TEST(testMakeBackInserterSink);
    CPPUNIT_TEST_SUITE_END();

public:
    void testMakeBackInserterSink();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MakeBackInserterSinkTest);

void MakeBackInserterSinkTest::testMakeBackInserterSink()
{
    std::vector<int> actual;
    std::vector<int> expected;
    auto sink = MakeBackInserterSink(&actual);

    for (int i = 0; i < 10; ++i) {
        sink.Receive(i);
        expected.push_back(i);
    }

    CPPUNIT_ASSERT(expected == actual);
}
