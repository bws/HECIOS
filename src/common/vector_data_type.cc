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
#include "vector_data_type.h"
#include <cassert>
#include <cmath>
using namespace std;

VectorDataType::VectorDataType(size_t count, size_t blockLength,
                               int32_t stride, const DataType& oldType)
    : DataType(calculateStridedExtent(count, blockLength,
                                      stride, oldType.getExtent())),
      count_(count),
      blockLength_(blockLength),
      stride_(stride),
      oldType_(oldType)
{
    assert(this != &oldType);
}

VectorDataType::VectorDataType(const VectorDataType& other)
    : DataType(other),
      count_(other.count_),
      blockLength_(other.blockLength_),
      stride_(other.stride_),
      oldType_(other.oldType_)
{
}

VectorDataType::~VectorDataType()
{
}

VectorDataType* VectorDataType::clone() const
{
    return new VectorDataType(*this);
}

size_t VectorDataType::getRepresentationByteLength() const
{
    return 4 + 4 + 4 + oldType_.getRepresentationByteLength();
}

vector<FileRegion> VectorDataType::getRegionsByBytes(const FSOffset& byteOffset,
                                                     size_t numBytes) const
{
    // The total regions produced
    vector<FileRegion> vectorRegions;

    // Construct the regions required to map numBytes of data to data regions
    size_t bytesProcessed = 0;
    FSOffset currentOffset = byteOffset;
    while (bytesProcessed < numBytes)
    {
        FSSize dataLength = min(blockLength_ * oldType_.getExtent(),
                                numBytes - bytesProcessed);
        vector<FileRegion> elementRegions =
            oldType_.getRegionsByBytes(currentOffset, dataLength);

        // Add regions to the total regions vector
        copy(elementRegions.begin(),
             elementRegions.end(),
             back_inserter(vectorRegions));

        // Update bookkeeping
        bytesProcessed += dataLength;
        currentOffset += stride_ * oldType_.getExtent();
    }
    assert(bytesProcessed == numBytes);
    return vectorRegions;
}

vector<FileRegion> VectorDataType::getRegionsByCount(const FSOffset& byteOffset,
                                                     size_t count) const
{
    // The total regions produced for count vectors
    vector<FileRegion> vectorRegions(count * count_);
    vector<FileRegion>::iterator vectorIter = vectorRegions.begin();

     // Flatten the types in order to construct count of the new type
     for (size_t i = 0; i < count; i++)
     {
         FSOffset countOffset = i * getExtent();
         for (size_t j = 0; j < count_; j++)
         {
             FSOffset nextOffset = byteOffset + countOffset +
                 j * stride_ * oldType_.getExtent();
             vector<FileRegion> elementRegions = oldType_.getRegionsByCount(
                 nextOffset, blockLength_);
             vectorIter = copy(elementRegions.begin(),
                               elementRegions.end(),
                               vectorIter);
         }
     }

     return vectorRegions;
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
