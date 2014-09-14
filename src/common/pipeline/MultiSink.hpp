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

#ifndef _TIBEE_COMMON_MULTISINK_HPP
#define _TIBEE_COMMON_MULTISINK_HPP

#include <vector>

#include <common/pipeline/ISink.hpp>

namespace tibee
{
namespace common
{

/**
 * A sink that forwards the elements it receives to multiple sinks.
 *
 * @author Francois Doray
 */
template <typename T>
class MultiSink
    : public ISink<T>
{
public:
    typedef ISink<T> SinkType;

    MultiSink() {}

    /*
     * Add a new sink to which elements will be forwarded.
     *
     * @params sink A sink to which elements will be forwarded.
     */
    void AddSink(SinkType* sink)
    {
        _sinks.push_back(sink);
    }

    /*
     * Overrides ISink.
     */
    virtual void Receive(const T& element) override
    {
      for (auto sink : _sinks)
        sink->Receive(element);
    }

private:
    // The registered sinks.
    typedef std::vector<SinkType*> SinkVector;
    SinkVector _sinks;


};

}  // namespace common
}  // namespace tibee

#endif  // _TIBEE_COMMON_MULTISINK_HPP
