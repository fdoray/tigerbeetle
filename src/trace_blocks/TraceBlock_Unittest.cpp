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
#include "block/BlockRunner.hpp"
#include "gtest/gtest.h"
#include "trace_blocks/TraceBlock.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace trace_blocks
{

namespace
{

// A block that counts events.
class CountBlock : public block::AbstractBlock
{
public:
    CountBlock() :
        _sched_switch_count {0},
        _starting_count {0},
        _loop_count {0},
        _done_count {0}
    {
    }

    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override
    {
        namespace pl = std::placeholders; 

        notificationCenter->AddObserver({
            notification::Token("event"),
            notification::RegexToken("^lttng-kernel$"),
            notification::Token("sched_switch")
        }, std::bind(&CountBlock::OnSchedSwitch, this, pl::_1, pl::_2));

        notificationCenter->AddObserver({
            notification::Token("event"),
            notification::Token("lttng-ust"),
            notification::Token("ust_tests_demo:starting")
        }, std::bind(&CountBlock::OnStarting, this, pl::_1, pl::_2));

        notificationCenter->AddObserver({
            notification::Token("event"),
            notification::Token("lttng-ust"),
            notification::Token("ust_tests_demo2:loop")
        }, std::bind(&CountBlock::OnLoop, this, pl::_1, pl::_2));

        notificationCenter->AddObserver({
            notification::Token("event"),
            notification::Token("lttng-ust"),
            notification::Token("ust_tests_demo:done")
        }, std::bind(&CountBlock::OnDone, this, pl::_1, pl::_2));

        notificationCenter->AddObserver({
            notification::Token("event"),
            notification::Token("lttng-ust"),
            notification::Token("ust_tests_demo3:done")
        }, std::bind(&CountBlock::OnDone, this, pl::_1, pl::_2));
    }

    void OnSchedSwitch(const notification::KeyPath& path, const value::Value* value)
    {
        ++_sched_switch_count;
    }

    void OnStarting(const notification::KeyPath& path, const value::Value* value)
    {
        ++_starting_count;
    }

    void OnLoop(const notification::KeyPath& path, const value::Value* value)
    {
        ++_loop_count;
    }

    void OnDone(const notification::KeyPath& path, const value::Value* value)
    {
        ++_done_count;
    }

    size_t _sched_switch_count;
    size_t _starting_count;
    size_t _loop_count;
    size_t _done_count;
};

}  // namespace

TEST(TraceBlock, kernel)
{
    value::StructValue traceParams;
    value::ArrayValue::UP traceList {new value::ArrayValue};
    traceList->Append<value::StringValue>("test_data/kernel_sched_switch/kernel");
    traceParams.AddField("traces", std::move(traceList));

    TraceBlock traceBlock;
    CountBlock countBlock;

    block::BlockRunner blockRunner;
    blockRunner.AddBlock(&traceBlock, &traceParams);
    blockRunner.AddBlock(&countBlock, nullptr);
    blockRunner.Run();

    EXPECT_EQ(1107, countBlock._sched_switch_count);
    EXPECT_EQ(0, countBlock._starting_count);
    EXPECT_EQ(0, countBlock._loop_count);
    EXPECT_EQ(0, countBlock._done_count);
}

TEST(TraceBlock, ust)
{
    value::StructValue traceParams;
    value::ArrayValue::UP traceList {new value::ArrayValue};
    traceList->Append<value::StringValue>("test_data/ust_a/ust/uid/1000/64-bit");
    traceParams.AddField("traces", std::move(traceList));

    TraceBlock traceBlock;
    CountBlock countBlock;

    block::BlockRunner blockRunner;
    blockRunner.AddBlock(&traceBlock, &traceParams);
    blockRunner.AddBlock(&countBlock, nullptr);
    blockRunner.Run();

    EXPECT_EQ(0, countBlock._sched_switch_count);
    EXPECT_EQ(1, countBlock._starting_count);
    EXPECT_EQ(5, countBlock._loop_count);
    EXPECT_EQ(2, countBlock._done_count);
}

}  // namespace trace_blocks
}  // namespace tibee
