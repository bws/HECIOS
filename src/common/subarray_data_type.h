#ifndef SUBARRAY_DATA_TYPE_H_
#define SUBARRAY_DATA_TYPE_H_
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
 * A subarray file view data type (similar to MPI data types)
 */
class SubarrayDataType : public DataType
{
public:
    /**
     * Array ordering C_ORDER corresponds to row major, FORTRAN_ORDER
     * corresponds to column major.
     */
    enum ArrayOrder {INVALID_ORDER = 0, C_ORDER = 1, FORTRAN_ORDER = 2};

    /**
     * @return the size of an n-dimension array
     *
     * @param sizes - number of elements in each dimension
     */
    static std::size_t calculateExtent(std::vector<std::size_t> sizes,
                                       const DataType& oldDataType);

    /** Constructor */
    SubarrayDataType(std::vector<std::size_t> sizes,
                     std::vector<std::size_t> subSizes,
                     std::vector<std::size_t> starts,
                     ArrayOrder arrayOrder,
                     const DataType& oldType);

    /** Destructor */
    virtual ~SubarrayDataType();

    /** @return a copy of this */
    virtual SubarrayDataType* clone() const;

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
    SubarrayDataType(const SubarrayDataType& other);

private:
    /**
     * @return the number of contiguous regions in the array for this subarray
     */
    std::size_t getNumArrayRegions() const;

    /** @return the memory location for region number */
    std::size_t getArrayMemoryLocation(size_t region) const;

    /**
     * @return the number of elements in the contiguous dimension
     *   (dim 0 for column-major, the last dimension for row-major)
     */
    std::size_t getSubarrayContiguousCount() const;

    /**
     * @return the extent of just the data for this process
     */
    std::size_t getSubSizeExtent() const;

    /** Hidden assignment operator */
    SubarrayDataType& operator=(const SubarrayDataType& other);

    /** Sizes of each dimension in the array */
    std::vector<std::size_t> sizes_;

    /** Size of each sub-array dimension */
    std::vector<std::size_t> subSizes_;

    /** Offset in each array dimension to begin the sub-array*/
    std::vector<std::size_t> starts_;

    /** Storage order of the array */
    ArrayOrder order_;

    /** The old data type to aggregate */
    const DataType& oldType_;
};

#endif /*SUBARRAY_DATA_TYPE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
