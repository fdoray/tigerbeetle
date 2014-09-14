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
#ifndef _TIBEE_BASE_PRINT_HPP
#define _TIBEE_BASE_PRINT_HPP

#include <ostream>
#include <sstream>
#include <string>

#include "base/TermModifier.hpp"

namespace tibee
{
namespace common
{

/**
 * Outputs a colored error to standard error.
 *
 * Call tbendl() at the end of the output instead of using std::endl.
 *
 * @return Stream to use for output
 */
std::ostream& tberror();

/**
 * Outputs a colored warning to standard output.
 *
 * Call tbendl() at the end of the output instead of using std::endl.
 *
 * @return Stream to use for output
 */
std::ostream& tbwarn();

/**
 * Outputs a colored information to standard output.
 *
 * Call tbendl() at the end of the output instead of using std::endl.
 *
 * @return Stream to use for output
 */
std::ostream& tbinfo();

/**
 * Outputs a module message to standard output.
 *
 * @param moduleName Module name
 * @return           Stream to use for output
 */
std::ostream& tbmsg(const std::string& moduleName);

/**
 * Terminates a colored output.
 *
 * @return String to output to terminate the colored output
 */
std::string tbendl();

}
}

#endif // _TIBEE_BASE_PRINT_HPP
