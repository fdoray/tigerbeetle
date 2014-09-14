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
#ifndef _TIBEE_TRACE_FIELDINFOS_HPP
#define _TIBEE_TRACE_FIELDINFOS_HPP

#include <unordered_map>
#include <memory>
#include <boost/filesystem.hpp>

#include "trace/BasicTypes.hpp"

namespace tibee
{
namespace trace
{

/**
 * (Declarative) informations about a field (within a dictionary or
 * within an event).
 *
 * FieldInfos objects are immutable, built once a trace is succesfully
 * added to a trace set.
 *
 * @see TraceSet
 *
 * @author Philippe Proulx
 */
class FieldInfos
{
public:
    /// (field name -> field infos) map
    typedef std::unordered_map<std::string, std::unique_ptr<FieldInfos>> FieldMap;

public:
    /**
     * Builds field informations.
     *
     * Pass \a nullptr to \p fieldMap if this field is not the parent
     * of any other field.
     *
     * @param index    Field index within its scope
     * @param name     Field name (unique within its scope)
     * @param fieldMap Map of field names to field infos (or \a nullptr)
     */
    FieldInfos(field_index_t index, const std::string& name,
               std::unique_ptr<FieldMap> fieldMap);

    /**
     * Returns the field index within its scope.
     *
     * @returns Field index
     */
    field_index_t getIndex() const
    {
        return _index;
    }

    /**
     * Returns the field name.
     *
     * @returns Field name
     */
    const std::string& getName() const
    {
        return _name;
    }

    /**
     * Returns the field map of this field.
     *
     * A field map maps field names to field infos.
     *
     * This method returns \a nullptr if this field is not the parent
     * of any other field.
     *
     * @returns Field map
     */
    const std::unique_ptr<FieldMap>& getFieldMap() const
    {
        return _fieldMap;
    }

private:
    field_index_t _index;
    std::string _name;
    std::unique_ptr<FieldMap> _fieldMap;
};

}
}

#endif // _TIBEE_TRACE_FIELDINFOS_HPP