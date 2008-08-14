#ifndef VECTOR_DATA_TYPE_H
#define VECTOR_DATA_TYPE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
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
#include "data_type.h"

/**
 * A vector file view data type (similar to MPI data types)
 */
class VectorDataType : public DataType
{
public:
    /** Default constructor */
    VectorDataType(std::size_t count,
                   std::size_t blockLength,
                   int stride,
                   const DataType& oldType);

    /** Destructor */
    virtual ~VectorDataType();

    /** @return a copy of this */
    virtual VectorDataType* clone() const;
    
    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;
    
   /**
     * @return the *data* regions for numBytes of data in this DataType.
     * Note that the DataType may contain empty holes, thus leading to
     * numBytes of data corresponding to a much larger DataType extent.
     */
    virtual std::vector<FileRegion> getRegionsByBytes(
        const FSOffset& byteOffset, std::size_t numBytes) const;

protected:
    /** Copy constructor for use by clone */
    VectorDataType(const VectorDataType& other);

private:
    /** Hidden assignment operator */
    VectorDataType& operator=(const VectorDataType& other);

    /** The number of blocks */
    std::size_t count_;

    /** Number of elements in each block */
    std::size_t blockLength_;

    /** Spacing between the start of each block, measured as the number of
        oldTypes (may be negative) */
    int stride_;
    
    /** The old type to aggregate */
    const DataType& oldType_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
