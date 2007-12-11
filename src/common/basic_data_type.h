#ifndef BASIC_DATA_TYPE_H
#define BASIC_DATA_TYPE_H
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
 * A basic file view data type for storing data of a specific bit width
 * (e.g. MPI_BYTE, MPI_INT, MPI_DOUBLE, etc.)
 */
class BasicDataType : public DataType
{
public:
    /** Byte width of the MPI_BYTE data type */
    static const std::size_t MPI_BYTE_WIDTH = 1;
    
    /** Byte width of the MPI_CHAR data type */
    static const std::size_t MPI_CHAR_WIDTH = 1;

    /** Byte width of the MPI_UNSIGNED_CHAR data type */
    static const std::size_t MPI_UNSIGNED_CHAR_WIDTH = 1;

    /** Byte width of the MPI_SHORT data type */
    static const std::size_t MPI_SHORT_WIDTH = 2;

    /** Byte width of the MPI_UNSIGNED_SHORT data type */
    static const std::size_t MPI_UNSIGNED_SHORT_WIDTH = 2;

    /** Byte width of the MPI_WIDE_CHAR data type */
    static const std::size_t MPI_WIDE_CHAR_WIDTH = 2;

    /** Byte width of the MPI_INT data type */
    static const std::size_t MPI_INT_WIDTH = 4;

    /** Byte width of the MPI_LONG data type */
    static const std::size_t MPI_LONG_WIDTH = 4;

    /** Byte width of the MPI_UNSIGNED data type */
    static const std::size_t MPI_UNSIGNED_WIDTH = 4;

    /** Byte width of the MPI_FLOAT data type */
    static const std::size_t MPI_FLOAT_WIDTH = 4;
    
    /** Byte width of the MPI_DOUBLE data type */
    static const std::size_t MPI_DOUBLE_WIDTH = 8;
    
    /** Byte width of the MPI_LONG_LONG data type */
    static const std::size_t MPI_LONG_LONG_WIDTH = 8;
    
    /** Default constructor */
    BasicDataType(std::size_t basicTypeBytes);

    /** Destructor */
    virtual ~BasicDataType();
    
    /** @return the number of bytes required to represent this data type */
    virtual std::size_t getRepresentationByteLength() const;
    
    /** @return the file regions in the data typefrom
        [byteOffset, byteOffset + byteLength] */
    virtual std::vector<FileRegion> getRegions(const FSOffset& byteOffset,
                                               std::size_t count) const;
    
private:
    /** Hidden copy constructor */
    BasicDataType(const BasicDataType& other);

    /** Hidden assignment operator */
    BasicDataType& operator=(const BasicDataType& other);
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
