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
#include <set>
#include <string>

#include "block/BlockRunner.hpp"
#include "gtest/gtest.h"
#include "state_blocks/CurrentStateBlock.hpp"
#include "state_blocks/LinuxSchedStateBlock.hpp"
#include "trace_blocks/TraceBlock.hpp"

namespace tibee
{
namespace state_blocks
{

TEST(LinuxSchedStateBlock, LinuxSchedStateBlock)
{
    value::StructValue traceParams;
    value::ArrayValue::UP traceList {new value::ArrayValue};
    traceList->Append<value::StringValue>("test_data/kernel_sched_switch/kernel");
    traceParams.AddField("traces", std::move(traceList));

    trace_blocks::TraceBlock traceBlock;
    CurrentStateBlock currentStateBlock;
    LinuxSchedStateBlock linuxBlock;

    block::BlockRunner blockRunner;
    blockRunner.AddBlock(&traceBlock, &traceParams);
    blockRunner.AddBlock(&currentStateBlock, nullptr);
    blockRunner.AddBlock(&linuxBlock, nullptr);
    blockRunner.Run();
}

}  // namespace state
}  // namespace tibee
