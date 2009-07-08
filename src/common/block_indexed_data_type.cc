//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include "block_indexed_data_type.h"
using namespace std;

BlockIndexedDataType::BlockIndexedDataType(const size_t& blockLength,
                                           const vector<size_t>& displacements,
                                           const DataType& oldDataType)
    : IndexedDataType(vector<size_t>(displacements.size(), blockLength),
                      displacements,
                      oldDataType)
{
}

BlockIndexedDataType::BlockIndexedDataType(const BlockIndexedDataType& other)
    : IndexedDataType(other)
{
}

BlockIndexedDataType::~BlockIndexedDataType()
{
}

BlockIndexedDataType* BlockIndexedDataType::clone() const
{
    return new BlockIndexedDataType(*this);
}

size_t BlockIndexedDataType::getRepresentationByteLength() const
{
    return IndexedDataType::getRepresentationByteLength();
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
