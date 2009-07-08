#ifndef BLOCK_INDEXED_DATA_TYPE_H_
#define BLOCK_INDEXED_DATA_TYPE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
