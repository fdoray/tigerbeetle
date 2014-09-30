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

class MockObserver
{
public:
    MOCK_METHOD2(method, void(const KeyPath& path,
                              const value::Value* value));
};
}  // namespace

TEST(NotificationCenter, simpleNotifications)
{
    namespace pl = std::placeholders;

    NotificationCenter notificationCenter;

    KeyPath path_a {Token("a")};
    KeyPath path_b {Token("a"), Token("b")};
    KeyPath path_c {Token("a"), Token("b"), Token("c")};
    KeyPath path_z {Token("y"), Token("z")};
    KeyPath path_y {Token("y")};

    NotificationSink* sink_a = notificationCenter.GetNotificationSink(path_a);
    NotificationSink* sink_b = notificationCenter.GetNotificationSink(path_b);
    NotificationSink* sink_c = notificationCenter.GetNotificationSink(path_c);
    NotificationSink* sink_z = notificationCenter.GetNotificationSink(path_z);
    NotificationSink* sink_y = notificationCenter.GetNotificationSink(path_y);

    MockObserver observer_a;
    MockObserver observer_b;
    MockObserver observer_c;
    MockObserver observer_z;
    MockObserver observer_y;

    notificationCenter.RegisterNotificationObserver(
        path_a, std::bind(&MockObserver::method, &observer_a, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_b, std::bind(&MockObserver::method, &observer_b, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_c, std::bind(&MockObserver::method, &observer_c, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_z, std::bind(&MockObserver::method, &observer_z, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_y, std::bind(&MockObserver::method, &observer_y, pl::_1, pl::_2));

    value::IntValue value(42);

    {
        EXPECT_CALL(observer_a, method(path_a, &value)).Times(1);
        EXPECT_CALL(observer_b, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_c, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_a, &value)).Times(0);
        sink_a->PostNotification(&value);
    }

    {
        EXPECT_CALL(observer_a, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_b, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_c, method(path_b, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_b, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_b, &value)).Times(0);
        sink_b->PostNotification(&value);
    }

    {
        EXPECT_CALL(observer_a, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_b, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_c, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_z, method(path_c, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_c, &value)).Times(0);
        sink_c->PostNotification(&value);
    }

    {
        EXPECT_CALL(observer_a, method(path_z, &value)).Times(0);
        EXPECT_CALL(observer_b, method(path_z, &value)).Times(0);
        EXPECT_CALL(observer_c, method(path_z, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_z, &value)).Times(1);
        EXPECT_CALL(observer_y, method(path_z, &value)).Times(1);
        sink_z->PostNotification(&value);
    }

    {
        EXPECT_CALL(observer_a, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_b, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_c, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_z, method(path_y, &value)).Times(0);
        EXPECT_CALL(observer_y, method(path_y, &value)).Times(1);
        sink_y->PostNotification(&value);
    }
}

TEST(NotificationCenter, regexNotifications)
{
    namespace pl = std::placeholders;

    NotificationCenter notificationCenter;

    KeyPath path_a {Token("aa")};
    KeyPath path_b {Token("aa"), Token("b1")};
    KeyPath path_c {Token("aa"), Token("b2")};
    KeyPath path_d {Token("ab"), Token("b1")};

    NotificationSink* sink_a = notificationCenter.GetNotificationSink(path_a);
    NotificationSink* sink_b = notificationCenter.GetNotificationSink(path_b);
    NotificationSink* sink_c = notificationCenter.GetNotificationSink(path_c);
    NotificationSink* sink_d = notificationCenter.GetNotificationSink(path_d);

    KeyPath path_all {RegexToken("^a")};
    KeyPath path_aa_b {Token("aa"), RegexToken("^b")};
    KeyPath path_all_b {RegexToken(".*"), RegexToken("^b")};
    KeyPath path_all_b1 {RegexToken(".*"), Token("b1")};
    KeyPath path_all_b2 {RegexToken(".*"), Token("b2")};

    MockObserver observer_all;
    MockObserver observer_aa_b;
    MockObserver observer_all_b;
    MockObserver observer_all_b1;
    MockObserver observer_all_b2;

    notificationCenter.RegisterNotificationObserver(
        path_all, std::bind(&MockObserver::method, &observer_all, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_aa_b, std::bind(&MockObserver::method, &observer_aa_b, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_all_b, std::bind(&MockObserver::method, &observer_all_b, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_all_b1, std::bind(&MockObserver::method, &observer_all_b1, pl::_1, pl::_2));

    notificationCenter.RegisterNotificationObserver(
        path_all_b2, std::bind(&MockObserver::method, &observer_all_b2, pl::_1, pl::_2));

    value::IntValue value(42);

    {
        EXPECT_CALL(observer_all, method(path_a, &value)).Times(1);
        EXPECT_CALL(observer_aa_b, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_all_b, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_all_b1, method(path_a, &value)).Times(0);
        EXPECT_CALL(observer_all_b2, method(path_a, &value)).Times(0);
        sink_a->PostNotification(&value);
    }

    {
        EXPECT_CALL(observer_all, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_aa_b, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_all_b, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_all_b1, method(path_b, &value)).Times(1);
        EXPECT_CALL(observer_all_b2, method(path_b, &value)).Times(0);
        sink_b->PostNotification(&value);
    }

    {
        EXPECT_CALL(observer_all, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_aa_b, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_all_b, method(path_c, &value)).Times(1);
        EXPECT_CALL(observer_all_b1, method(path_c, &value)).Times(0);
        EXPECT_CALL(observer_all_b2, method(path_c, &value)).Times(1);
        sink_c->PostNotification(&value);
    }

    {
        EXPECT_CALL(observer_all, method(path_d, &value)).Times(1);
        EXPECT_CALL(observer_aa_b, method(path_d, &value)).Times(0);
        EXPECT_CALL(observer_all_b, method(path_d, &value)).Times(1);
        EXPECT_CALL(observer_all_b1, method(path_d, &value)).Times(1);
        EXPECT_CALL(observer_all_b2, method(path_d, &value)).Times(0);
        sink_d->PostNotification(&value);
    }
}

}  // namespace notification
}  // namespace tibee
