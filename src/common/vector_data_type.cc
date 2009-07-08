//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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

    // Calculate the stride in bytes
    size_t strideLength = stride_ * oldType_.getTrueExtent();

    // Construct the regions required to map numBytes of data to data regions
    size_t bytesProcessed = 0;
    FSOffset currentOffset = byteOffset;
    while (bytesProcessed < numBytes)
    {
        // Initialize the data length to the full size for this vector
        FSSize dataLength = blockLength_ * oldType_.getTrueExtent();

        // Handle the partial region that may be at the beginning  of this
        // data type
        size_t offsetInStride = currentOffset % strideLength;
        if (0 != offsetInStride)
        {
            // If the offset is after all of the data in the vector,
            // just adjust the current offset
            // Else determine the partial data length
            if (offsetInStride > dataLength)
            {
                currentOffset += strideLength - offsetInStride;
            }
            else
            {
                dataLength = dataLength - offsetInStride;
            }
        }

        // Trim the data size to the requested size if neccesary
        dataLength = min(dataLength, numBytes - bytesProcessed);

        // Retrieve the regions for the embedded type
        vector<FileRegion> elementRegions =
        oldType_.getRegionsByBytes(currentOffset, dataLength);

        // Add regions to the total regions vector
        copy(elementRegions.begin(),
             elementRegions.end(),
             back_inserter(vectorRegions));

        // Update bytes processed, and set the offset to the beginning of the
        // next vector beginning
        bytesProcessed += dataLength;
        currentOffset = ((currentOffset / strideLength) + 1) * strideLength;
    }
    assert(bytesProcessed == numBytes);
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
