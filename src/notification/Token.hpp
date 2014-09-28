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
#ifndef _TIBEE_NOTIFICATION_TOKEN_HPP
#define _TIBEE_NOTIFICATION_TOKEN_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <string>

namespace tibee
{
namespace notification
{

/**
 * Notification key path token.
 *
 * @author Francois Doray
 */
class Token {
public:
    Token(const std::string& token)
        : _token(token),
          _isRegex(false)
    {
    }

    Token(const std::string& token, bool isRegex)
        : _token(token),
          _isRegex(isRegex)
    {
    }

    const std::string& token() const { return _token; }
    bool isRegex() const { return _isRegex; }

    Token& operator=(const std::string token)
    {
        _token = token;
        _isRegex = false;
        return *this;
    }
    bool operator==(const Token& other) const
    {
        return _token == other._token && _isRegex == other._isRegex;
    }

private:
    std::string _token;
    bool _isRegex;
};

inline size_t hash_value(const Token& token)
{
    std::hash<std::string> str_hash;
    return str_hash(token.token());
}

inline Token RegexToken(const std::string& token)
{
    return Token(token, true);
}

}
}

#endif // _TIBEE_NOTIFICATION_TOKEN_HPP
