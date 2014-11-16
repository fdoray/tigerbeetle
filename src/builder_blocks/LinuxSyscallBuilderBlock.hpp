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
#ifndef _TIBEE_BUILDERBLOCKS_LINUXSYSCALLBUILDERBLOCK_HPP
#define _TIBEE_BUILDERBLOCKS_LINUXSYSCALLBUILDERBLOCK_HPP

#include "builder_blocks/AbstractBuilderBlock.hpp"
#include "notification/Path.hpp"

namespace tibee {
namespace builder_blocks {

/**
 * Linux system calls builder block.
 *
 * @author Francois Doray
 */
class LinuxSyscallBuilderBlock : public AbstractBuilderBlock
{
public:
    LinuxSyscallBuilderBlock();

    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onSyscall(uint32_t tid, const notification::Path& path, const value::Value* value);
};

}  // namespace builder_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDERBLOCKS_LINUXSYSCALLBUILDERBLOCK_HPP
