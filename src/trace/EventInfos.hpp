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
#ifndef _TIBEE_TRACE_EVENTINFOS_HPP
#define _TIBEE_TRACE_EVENTINFOS_HPP

#include <memory>

#include "base/BasicTypes.hpp"
#include "trace/FieldInfos.hpp"

namespace tibee
{
namespace trace
{

/**
 * (Declarative) informations about an event.
 *
 * EventInfos objects are immutable, built once a trace is succesfully
 * added to a trace set.
 *
 * @see TraceSet
 *
 * @author Philippe Proulx
 */
class EventInfos
{
public:
    /**
     * Builds event informations.
     *
     * @param id       Event ID (unique within a trace)
     * @param name     Event name (unique within a trace)
     * @param fieldMap Map of field names to field infos
     */
    EventInfos(event_id_t id, const std::string& name,
               std::unique_ptr<FieldInfos::FieldMap> fieldMap);

    /**
     * Returns the event ID.
     *
     * @returns Event ID
     */
    event_id_t getId() const
    {
        return _id;
    }

    /**
     * Returns the event name.
     *
     * @returns Event name
     */
    const std::string& getName() const
    {
        return _name;
    }

    /**
     * Returns the event field map.
     *
     * A field map maps field names to field infos.
     *
     * @returns Event field map
     */
    const std::unique_ptr<FieldInfos::FieldMap>& getFieldMap() const
    {
        return _fieldMap;
    }

private:
    event_id_t _id;
    std::string _name;
    std::unique_ptr<FieldInfos::FieldMap> _fieldMap;
};

}
}

#endif // _TIBEE_TRACE_EVENTINFOS_HPP