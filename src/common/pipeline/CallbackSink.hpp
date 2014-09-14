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

#ifndef _TIBEE_COMMON_CALLBACKSINK_HPP
#define _TIBEE_COMMON_CALLBACKSINK_HPP

#include <common/pipeline/ISink.hpp>

namespace tibee
{
namespace common
{

/**
 * A sink that forwards the elements it receives to a callback method.
 *
 * @author Francois Doray
 */
template <typename T, typename O>
class CallbackSink
    : public ISink<T>
{
public:
    typedef void (O::*Callback)(const T&);

    /*
     * Constructor.
     *
     * @param object The object on which the callback is invoked.
     * @param method The callback method.
     */
    CallbackSink(O* object, Callback method)
        : object_(object), method_(method) {}

    /*
     * Overrides ISink.
     */
    virtual void Receive(const T& element) override
    {
      (object_->*method_)(element);
    }

private:
    // Object on which the callback is invoked.
    O* object_;

    // The callback method.
    Callback method_;
};

}  // namespace common
}  // namespace tibee

#endif  // _TIBEE_COMMON_CALLBACKSINK_HPP
