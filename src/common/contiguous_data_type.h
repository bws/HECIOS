#ifndef CONTIGUOUS_DATA_TYPE_H
#define CONTIGUOUS_DATA_TYPE_H
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
 * A contiguous file view data type (similar to MPI data types)
 */
class ContiguousDataType : public DataType
{
public:
    /** Default constructor */
    ContiguousDataType(std::size_t count, const DataType& oldType);

    /** Destructor */
    virtual ~ContiguousDataType();
    
    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;
    
    /** @return the file regions in the data typefrom
        [byteOffset, byteOffset + byteLength] */
    std::vector<FileRegion> getRegions(const FSOffset& byteOffset,
                                       std::size_t count) const;
    
private:
    /** Hidden copy constructor */
    ContiguousDataType(const ContiguousDataType& other);

    /** Hidden assignment operator */
    ContiguousDataType& operator=(const ContiguousDataType& other);

    /** The count of contiguous old types */
    std::size_t count_;

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
