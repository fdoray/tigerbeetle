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
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "notification/NotificationCenter.hpp"
#include "notification/NotificationSink.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace notification
{

namespace
{
using testing::Ref;
typedef NotificationCenter::KeyPath KeyPath;

class MockObserver
{
public:
    MOCK_METHOD2(method, void(const KeyPath& path,
                              const value::Value* value));
};
}  // namespace

TEST(NotificationCenter, notificationCenter)
{
    namespace pl = std::placeholders;

    NotificationCenter notificationCenter;

    KeyPath path_a {"a"};
    KeyPath path_b {"a", "b"};
    KeyPath path_c {"a", "b", "c"};
    KeyPath path_z {"y", "z"};
    KeyPath path_y {"y"};

    MockObserver observer_a;
    MockObserver observer_b;
    MockObserver observer_c;
    MockObserver observer_z;
    MockObserver observer_y;

    notificationCenter.RegisterObserver(
        path_a, std::bind(&MockObserver::method, &observer_a, pl::_1, pl::_2));

    notificationCenter.RegisterObserver(
        path_b, std::bind(&MockObserver::method, &observer_b, pl::_1, pl::_2));

    notificationCenter.RegisterObserver(
        path_c, std::bind(&MockObserver::method, &observer_c, pl::_1, pl::_2));

    notificationCenter.RegisterObserver(
        path_z, std::bind(&MockObserver::method, &observer_z, pl::_1, pl::_2));

    notificationCenter.RegisterObserver(
        path_y, std::bind(&MockObserver::method, &observer_y, pl::_1, pl::_2));

    value::IntValue value(42);

    NotificationSink* sink_a = notificationCenter.GetSink(path_a);
    {
        EXPECT_CALL(observer_a, method(path_a, &value)).Times(1);
        EXPECT_CALL(observer_b, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_c, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_a, &value)).Times(0);
        sink_a->PostNotification(&value);
    }

    NotificationSink* sink_b = notificationCenter.GetSink(path_b);
    {
        EXPECT_CALL(observer_a, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_b, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_c, method(path_b, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_b, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_b, &value)).Times(0);
        sink_b->PostNotification(&value);
    }

    NotificationSink* sink_c = notificationCenter.GetSink(path_c);
    {
        EXPECT_CALL(observer_a, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_b, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_c, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_z, method(path_c, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_c, &value)).Times(0);
        sink_c->PostNotification(&value);
    }

    NotificationSink* sink_z = notificationCenter.GetSink(path_z);
    {
        EXPECT_CALL(observer_a, method(path_z, &value)).Times(0);
        EXPECT_CALL(observer_b, method(path_z, &value)).Times(0);
        EXPECT_CALL(observer_c, method(path_z, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_z, &value)).Times(1);
        EXPECT_CALL(observer_y, method(path_z, &value)).Times(1);
        sink_z->PostNotification(&value);
    }

    NotificationSink* sink_y = notificationCenter.GetSink(path_y);
    {
        EXPECT_CALL(observer_a, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_b, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_c, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_y, &value)).Times(1);
        sink_y->PostNotification(&value);
    }
}

}  // namespace notification
}  // namespace tibee
