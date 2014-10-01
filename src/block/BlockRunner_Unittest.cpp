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

#include <vector>
#include <string>

#include "gtest/gtest.h"
#include "block/AbstractBlock.hpp"
#include "block/BlockRunner.hpp"
#include "block/ServiceList.hpp"
#include "value/Utils.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace block
{

namespace
{

struct SimpleService
{
    SimpleService() : count(0) {}
    int count;
};

// A block that registers a service.
class BlockA : public AbstractBlock
{
public:
    BlockA(SimpleService* service)
        : _service(service)
    {
    }

    virtual void RegisterServices(ServiceList* serviceList) override
    {
        serviceList->AddService("simpleService", _service);
    }

private:
    SimpleService* _service;
};

// A block that keeps track of the methods that are called on it
// and uses a service.
class BlockB : public AbstractBlock
{
public:
    virtual void Start(const value::Value* parameters) override
    {
        std::string parameters_str;
        if (value::ToString(parameters, &parameters_str)) {
            callHistory.push_back(std::string("Start with parameter ") + parameters_str);
        }
    }

    virtual void GetNotificationSinks(notification::NotificationCenter* notificationCenter) override
    {
        callHistory.push_back("GetNotificationSinks");
    }

    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override
    {
        callHistory.push_back("AddObservers");
    }

    virtual void RegisterServices(ServiceList* serviceList) override
    {
        callHistory.push_back("RegisterServices");
    }

    virtual void LoadServices(const ServiceList& serviceList) override
    {
        callHistory.push_back("LoadServices");

        SimpleService* simpleService = nullptr;
        if (serviceList.QueryService("simpleService", reinterpret_cast<void**>(&simpleService)))
            ++simpleService->count;

    }

    virtual void Execute() override
    {
        callHistory.push_back("Execute");
    }

    virtual void Stop() override
    {
        callHistory.push_back("Stop");
    }

    std::vector<std::string> callHistory;
};

}  // namespace

TEST(BlockRunner, run)
{
    BlockRunner blockRunner;

    SimpleService simpleService;
    BlockA blockA(&simpleService);
    BlockB blockB;
    value::IntValue param {42};

    blockRunner.AddBlock(&blockA, &param);
    blockRunner.AddBlock(&blockB, &param);

    blockRunner.Run();

    EXPECT_EQ(1, simpleService.count);

    std::vector<std::string> expectedHistory = {
        "Start with parameter 42",
        "GetNotificationSinks",
        "AddObservers",
        "RegisterServices",
        "LoadServices",
        "Execute",
        "Stop",
    };

    EXPECT_EQ(expectedHistory, blockB.callHistory);
}

}  // namespace block
}  // namespace tibee
