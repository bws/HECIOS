#ifndef STRUCT_DATA_TYPE_H_
#define STRUCT_DATA_TYPE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#include <cstddef>
#include <vector>
#include "data_type.h"

/**
 * A struct file view data type (similar to MPI data types)
 */
class StructDataType : public DataType
{
public:
    /**
     * @return the size of an n-dimension array
     *
     * @param sizes - number of elements in each dimension
     */
    static std::size_t calculateExtent(std::vector<std::size_t> blockLengths,
                                       std::vector<std::size_t> displacements,
                                       std::vector<DataType*> oldDataTypes);

    /** Constructor */
    StructDataType(std::vector<std::size_t> blockLengths,
                   std::vector<std::size_t> displacements,
                   std::vector<DataType*> oldTypes);

    /** Destructor */
    virtual ~StructDataType();

    /** @return a copy of this */
    virtual StructDataType* clone() const;

    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;

   /**
     * @return the *data* regions for numBytes of data in this DataType.
     * Note that the DataType may contain empty holes, thus leading to
     * numBytes of data corresponding to a much larger DataType extent.
     */
    virtual std::vector<FileRegion> getRegionsByBytes(
        const FSOffset& byteOffset, std::size_t numBytes) const;

    /**
     * @return the data regions for count of this DataType.
     */
    virtual std::vector<FileRegion> getRegionsByCount(
        const FSOffset& byteOffset, std::size_t count) const;

protected:
    /** Copy constructor for use by clone */
    StructDataType(const StructDataType& other);

private:
    /** Hidden assignment operator */
    StructDataType& operator=(const StructDataType& other);

    /** The data types to aggregate */
    std::vector<DataType*> types_;
};

#endif /*STRUCT_DATA_TYPE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
