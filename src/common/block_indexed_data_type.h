#ifndef BLOCK_INDEXED_DATA_TYPE_H_
#define BLOCK_INDEXED_DATA_TYPE_H_
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
#include "indexed_data_type.h"

/**
 * A indexed file view data type (similar to MPI data types)
 */
class BlockIndexedDataType : public IndexedDataType
{
public:
    /** Constructor */
    BlockIndexedDataType(const std::size_t& blocklength,
                         const std::vector<std::size_t>& displacements,
                         const DataType& oldType);

    /** Destructor */
    virtual ~BlockIndexedDataType();

    /** @return a copy of this */
    virtual BlockIndexedDataType* clone() const;

    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;

protected:
    /** Copy constructor for use by clone */
    BlockIndexedDataType(const BlockIndexedDataType& other);

private:
    /** Hidden assignment operator */
    BlockIndexedDataType& operator=(const BlockIndexedDataType& other);
};

#endif /*BLOCK_INDEXED_DATA_TYPE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
