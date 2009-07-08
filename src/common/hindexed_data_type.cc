//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include "hindexed_data_type.h"
using namespace std;

HindexedDataType::HindexedDataType(const vector<size_t>& blockLengths,
                                   const vector<size_t>& displacements,
                                   const DataType& oldDataType)
    : StructDataType(blockLengths,
                     displacements,
                     vector<const DataType*>(blockLengths.size(), &oldDataType))
{
}

HindexedDataType::HindexedDataType(const HindexedDataType& other)
  : StructDataType(other)
{
}

HindexedDataType::~HindexedDataType()
{

}

HindexedDataType* HindexedDataType::clone() const
{
    return new HindexedDataType(*this);
}

size_t HindexedDataType::getRepresentationByteLength() const
{
    size_t length = 4 + (4 + 4) * count() +
        oldType(0)->getRepresentationByteLength();
    return length;
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
