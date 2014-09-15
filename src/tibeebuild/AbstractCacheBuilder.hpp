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
#ifndef _ABSTRACTCACHEBUILDER_HPP
#define _ABSTRACTCACHEBUILDER_HPP

#include <memory>
#include <boost/filesystem/path.hpp>

#include <common/trace/TraceSet.hpp>
#include <common/trace/Event.hpp>
#include <common/traceplayback/AbstractTracePlaybackListener.hpp>

namespace tibee
{

/**
 * Abstract cache builder. All cache builders must inherit this.
 *
 * @author Philippe Proulx
 */
class AbstractCacheBuilder :
    public tibee::common::AbstractTracePlaybackListener
{
public:
    /**
     * Builds a cache builder.
     *
     * @param dir Cache directory
     */
    AbstractCacheBuilder(const boost::filesystem::path& dir);

    virtual ~AbstractCacheBuilder() = 0;

    /**
     * Returns the cache directory holding this cache.
     *
     * @returns Cache directory
     */
    const boost::filesystem::path& getCacheDir() const
    {
        return _cacheDir;
    }

private:
    boost::filesystem::path _cacheDir;
};

}

#endif // _ABSTRACTCACHEBUILDER_HPP
