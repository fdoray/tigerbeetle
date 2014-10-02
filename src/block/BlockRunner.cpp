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

#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"

namespace tibee
{
namespace block
{

BlockRunner::BlockRunner()
{
}

BlockRunner::~BlockRunner()
{
}

void BlockRunner::AddBlock(BlockInterface* block,
                           const value::Value* parameters)
{
    _blocks.push_back(std::make_pair(block, parameters));
}

void BlockRunner::Run()
{
    notification::NotificationCenter notificationCenter;
    block::ServiceList serviceList;

    // Notify the blocks that the execution will start.
    for (auto& block : _blocks)
        block.first->Start(block.second);

    // Ask the blocks to declare the services that they offer.
    for (auto& block : _blocks)
        block.first->RegisterServices(&serviceList);

    // Register the notification center.
    serviceList.AddService(
        notification::NotificationCenter::kNotificationCenterServiceName,
        &notificationCenter);

    // Let the blocks load services.
    for (auto& block : _blocks)
        block.first->LoadServices(serviceList);

    // Ask the blocks to declare the notifications that they receive.
    for (auto& block : _blocks)
        block.first->AddObservers(&notificationCenter);

    // Ask the blocks to declare the notifications that they produce.
    for (auto& block : _blocks)
        block.first->GetNotificationSinks(&notificationCenter);

    // Execute the blocks.
    for (auto& block : _blocks)
        block.first->Execute();

    // Stop the execution of the blocks.
    for (auto& block : _blocks)
        block.first->Stop();
}

}
}
