/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
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
#include <iostream>

#include "base/print.hpp"

namespace tibee
{
namespace base
{

std::ostream& tberror()
{
    return std::cerr << TM(TM::FG_RED) << "Error: " << TM(TM::BOLD);
}

std::ostream& tbwarn()
{
    return std::cout << TM(TM::FG_YELLOW) << "Warning: " << TM(TM::BOLD);
}

std::ostream& tbinfo()
{
    return std::cout << TM(TM::FG_BLUE) << "Warning: " << TM(TM::BOLD);
}

std::ostream& tbmsg(const std::string& moduleName)
{
    std::cout << TM(TM::BOLD) << TM(TM::FG_GREEN) << moduleName <<
                 TM(TM::NOBOLD) << ": " << TM(TM::RESET);

    return std::cout;
}

std::string tbendl()
{
    std::stringstream ss;

    ss << TM(TM::RESET) << std::endl;

    return ss.str();
}

}
}
