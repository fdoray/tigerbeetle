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
#ifndef _TIBEE_BASE_TERMMODIFIER_HPP
#define _TIBEE_BASE_TERMMODIFIER_HPP

#include <ostream>

namespace tibee
{
namespace base
{

/**
 * Terminal modifier, used to output colored text and such.
 *
 * @author Philippe Proulx
 */
class TermModifier
{
public:
    /// Codes to use for building modifiers
    enum Code {
        RESET = 0,
        BOLD = 1,
        NOBOLD = 21,
        DIM = 2,
        NODIM = 22,
        REVERSE = 7,
        NOREVERSE = 27,
        FG_DEF = 39,
        FG_BLACK = 30,
        FG_RED = 31,
        FG_GREEN = 32,
        FG_YELLOW = 33,
        FG_BLUE = 34,
        FG_MAGENTA = 35,
        FG_CYAN = 36,
        FG_GREY = 37,
        FG_WHITE = 97,
        BG_DEF = 49,
        BG_BLACK = 40,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_YELLOW = 43,
        BG_BLUE = 44,
        BG_MAGENTA = 45,
        BG_CYAN = 46,
        BG_GREY = 47,
        BG_WHITE = 107,
    };

public:
    /**
     * Builds a terminal modifier with the given code. Use one of
     * the public codes provided by TermModifier.
     *
     * @param code Terminal code to use
     */
    TermModifier(Code code) :
        _code {code}
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const TermModifier& mod)
    {
        return os << "\033[" << mod._code << "m";
    }

private:
    Code _code;
};

typedef TermModifier TM;

}
}

#endif // _TIBEE_COMMON_TERMMODIFIER_HPP
