//
// This file is part of Hecios
//
// Copyright (C) 2008 bradles
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
#include "subarray_data_type.h"
using namespace std;

size_t SubarrayDataType::calculateExtent(vector<size_t> sizes,
                                         const DataType& oldDataType)
{
    size_t extent = 0;
    for (size_t i = 0; i < sizes.size(); i++)
    {
        extent *= sizes[i] * oldDataType.getExtent();
    }
    return extent;
}

SubarrayDataType::SubarrayDataType(vector<size_t> sizes,
                                   vector<size_t> subSizes,
                                   vector<size_t> starts,
                                   SubarrayDataType::ArrayOrder ordering,
                                   const DataType& oldDataType)
  : DataType(SubarrayDataType::calculateExtent(sizes, oldDataType)),
    oldType_(oldDataType)
{

}

SubarrayDataType::SubarrayDataType(const SubarrayDataType& other)
    : DataType(other),
    oldType_(other.oldType_)
{

}

SubarrayDataType::~SubarrayDataType()
{

}

SubarrayDataType* SubarrayDataType::clone() const
{
    return new SubarrayDataType(*this);
}

size_t SubarrayDataType::getRepresentationByteLength() const
{
    return (4 + 4 + 4) + oldType_.getRepresentationByteLength();
}

vector<FileRegion> SubarrayDataType::getRegionsByBytes(const FSOffset& byteOffset,
                                                       size_t bytes) const
{
    vector<FileRegion> regions;
    return regions;
}

vector<FileRegion> SubarrayDataType::getRegionsByCount(const FSOffset& byteOffset,
                                                       size_t count) const
{
    vector<FileRegion> regions;
    return regions;
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
