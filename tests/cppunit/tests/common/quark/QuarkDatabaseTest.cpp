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
#include <string>

#include <common/quark/Quark.hpp>
#include <common/quark/QuarkDatabase.hpp>

using namespace tibee::common;

class QuarkDatabaseTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(QuarkDatabaseTest);
        CPPUNIT_TEST(testInsertInt);
        CPPUNIT_TEST(testInsertString);
        CPPUNIT_TEST(testInsertManyElements);
        CPPUNIT_TEST(testValueOfInt);
        CPPUNIT_TEST(testValueOfString);
    CPPUNIT_TEST_SUITE_END();

public:
    void testInsertInt();
    void testInsertString();
    void testInsertManyElements();
    void testValueOfInt();
    void testValueOfString();
};

CPPUNIT_TEST_SUITE_REGISTRATION(QuarkDatabaseTest);

void QuarkDatabaseTest::testInsertInt()
{
    QuarkDatabase<int> db;

    Quark one = db.Insert(1);
    Quark two = db.Insert(2);

    Quark other_one = db.Insert(1);
    Quark other_two = db.Insert(2);

    CPPUNIT_ASSERT_EQUAL(one.get(), other_one.get());
    CPPUNIT_ASSERT_EQUAL(two.get(), other_two.get());
}

void QuarkDatabaseTest::testInsertString()
{
    QuarkDatabase<std::string> db;

    Quark one = db.Insert("one");
    Quark two = db.Insert("two");

    Quark other_one = db.Insert("one");
    Quark other_two = db.Insert("two");

    CPPUNIT_ASSERT_EQUAL(one.get(), other_one.get());
    CPPUNIT_ASSERT_EQUAL(two.get(), other_two.get());
}

void QuarkDatabaseTest::testInsertManyElements()
{
    QuarkDatabase<int> db;
    std::vector<Quark> quarks;

    for (int i = 0; i < 256; ++i)
        quarks.push_back(db.Insert(i));

    for (int i = 0; i < 256; ++i) {
        const Quark& other_quark = db.Insert(i);
        CPPUNIT_ASSERT_EQUAL(quarks.at(i).get(), other_quark.get());
    }
}

void QuarkDatabaseTest::testValueOfInt()
{
    QuarkDatabase<int> db;

    Quark one = db.Insert(1);
    Quark two = db.Insert(2);

    const int& value_one = db.ValueOf(one);
    const int& value_two = db.ValueOf(two);

    CPPUNIT_ASSERT_EQUAL(1, value_one);
    CPPUNIT_ASSERT_EQUAL(2, value_two);
}

void QuarkDatabaseTest::testValueOfString()
{
    QuarkDatabase<std::string> db;

    Quark one = db.Insert("one");
    Quark two = db.Insert("two");

    const std::string& value_one = db.ValueOf(one);
    const std::string& value_two = db.ValueOf(two);

    CPPUNIT_ASSERT_EQUAL(std::string("one"), value_one);
    CPPUNIT_ASSERT_EQUAL(std::string("two"), value_two);
}
