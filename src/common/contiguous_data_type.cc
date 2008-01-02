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

vector<FileRegion> ContiguousDataType::getRegionsByCount(
    const FSOffset& byteOffset,
    size_t count) const
{
    return oldType_->getRegionsByCount(byteOffset, count * count_);
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
