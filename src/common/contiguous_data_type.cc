//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "contiguous_data_type.h"
#include <cassert>
using namespace std;

ContiguousDataType::ContiguousDataType(size_t count, const DataType& oldType)
    : DataType(count * oldType.getExtent()),
      count_(count),
      oldType_(oldType.clone())
{
    assert(this != &oldType);
}

ContiguousDataType::ContiguousDataType(const ContiguousDataType& other)
    : DataType(other),
      count_(other.count_),
      oldType_(other.oldType_)
{
}

ContiguousDataType::~ContiguousDataType()
{
    delete oldType_;
}

ContiguousDataType* ContiguousDataType::clone() const
{
    return new ContiguousDataType(*this);
}

size_t ContiguousDataType::getRepresentationByteLength() const
{
    return 4 + oldType_->getRepresentationByteLength();
}

vector<FileRegion> ContiguousDataType::getRegionsByBytes(
    const FSOffset& byteOffset,
    size_t numBytes) const
{
    return oldType_->getRegionsByBytes(byteOffset, numBytes);
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
