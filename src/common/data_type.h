#ifndef DATA_TYPE_H
#define DATA_TYPE_H
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
#include <stdint.h>
#include <vector>
#include "basic_types.h"

/**
 * An abstract file view data type (analogous to MPI data types)
 */
class DataType
{
public:
    /** Static helper fuction for computing the extent of strided data types */
    static std::size_t DataType::calculateStridedExtent(
        std::size_t count, std::size_t blockLength,
        int32_t stride, std::size_t elementExtent);

    /** Construct a data type with extent extent */
    DataType(std::size_t extent);

    /** Destructor */
    virtual ~DataType();

    /** @return a copy of the correct concrete derived class */
    virtual DataType* clone() const = 0;
    
    /** @return the size of a single element of this data type */
    std::size_t getExtent() const;

    /** @return the true size of a single element of this data type */
    std::size_t getTrueExtent() const;

    /** Sets the extent to lowerBound + extent */
    void resize(int lowerBound, std::size_t extent);
    
    /** @return the number of bytes required to represent this data type */
    virtual std::size_t getRepresentationByteLength() const = 0;

    /**
     * @return the data type regions in the data type from
     * [byteOffset, byteOffset + numBytes] dataType is this,
     * possibly non-contiguous, data type
     */
    virtual std::vector<FileRegion> getRegions(
        const FSOffset& byteOffset, const std::size_t numBytes) const = 0;

protected:
    /** Copy constructor may be called by clone */
    DataType(const DataType& other);
    
private:
    /** Hidden assignment operator */
    DataType& operator=(const DataType& other);

    /** The data type extent, may be resized to a false value */
    std::size_t extent_;

    /** The true data type extent */
    std::size_t trueExtent_;
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
