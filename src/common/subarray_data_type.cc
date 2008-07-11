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
    size_t extent = 1;
    for (size_t i = 0; i < sizes.size(); i++)
    {
        extent *= sizes[i] * oldDataType.getTrueExtent();
    }
    return extent;
}

SubarrayDataType::SubarrayDataType(vector<size_t> sizes,
                                   vector<size_t> subSizes,
                                   vector<size_t> starts,
                                   SubarrayDataType::ArrayOrder ordering,
                                   const DataType& oldDataType)
  : DataType(SubarrayDataType::calculateExtent(sizes, oldDataType)),
    sizes_(sizes),
    subSizes_(subSizes),
    starts_(starts),
    order_(ordering),
    oldType_(oldDataType)
{
    // Validate that subarray specification is valid
    for (size_t i = 0; i < sizes_.size(); i++)
    {
        assert(0 < sizes_[i]);
        assert(0 < subSizes_[i]);
        assert(starts_[i] + subSizes_[i] <= sizes_[i]);
    }
}

SubarrayDataType::SubarrayDataType(const SubarrayDataType& other)
  : DataType(other),
    sizes_(other.sizes_),
    subSizes_(other.subSizes_),
    starts_(other.starts_),
    order_(other.order_),
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
    return (4 + 4 + 4) * sizes_.size() +
        1 + oldType_.getRepresentationByteLength();
}

vector<FileRegion> SubarrayDataType::getRegionsByBytes(const FSOffset& byteOffset,
                                                       size_t numBytes) const
{
    vector<FileRegion> regions;

    // Determine the size of the contiguous data type region
    size_t contigCount = getSubarrayContiguousCount();
    size_t contigLength = contigCount * oldType_.getTrueExtent();

    size_t bytesProcessed = 0;
    FSOffset arrayOffset = byteOffset;
    while (bytesProcessed < numBytes)
    {
        // Determine the number discontiguous regions within the sub-array
        size_t firstRegion = arrayOffset / contigLength;
        size_t numRegions = getNumArrayRegions();

        // Process regions in the array until enough bytes are processed
        // or the array
        for (size_t i = firstRegion; i < numRegions && bytesProcessed < numBytes; i++)
        {
            // Calculate the memory offset into the sub-array
            FSOffset regionOffset = getArrayMemoryLocation(i);
            FSSize dataLength = contigCount * oldType_.getTrueExtent();

            // Account for the fist offset possibly occurring in the middle
            // of a contiguous region
            if (firstRegion == i &&
                byteOffset >= regionOffset &&
                byteOffset < FSOffset(regionOffset + contigLength))
            {
                dataLength = (regionOffset + contigLength) - byteOffset;
            }

            // Trim the data length if the entire length isn't needed
            dataLength = min(dataLength, numBytes = bytesProcessed);

            // Get region data
            vector<FileRegion> elementRegions =
                oldType_.getRegionsByBytes(arrayOffset + regionOffset,
                                            dataLength);

            // Add regions to the total regions vector
            copy(elementRegions.begin(),
                 elementRegions.end(),
                 back_inserter(regions));

            // Update the number of bytes processed
            bytesProcessed += dataLength;
        }

        // Increment the array offset to the next subarray
        arrayOffset = ((arrayOffset/getTrueExtent()) + 1) * getTrueExtent();
    }
    return regions;
}

vector<FileRegion> SubarrayDataType::getRegionsByCount(const FSOffset& byteOffset,
                                                       size_t count) const
{
    assert(false);
    vector<FileRegion> regions;
    return regions;
}

size_t SubarrayDataType::getNumArrayRegions() const
{
    // Determine the number discontiguous regions within the sub-array
    size_t arrayRegions = 0;

    if (0 != subSizes_.size())
    {
        arrayRegions = 1;
        if (C_ORDER == order_)
        {
            for (size_t i = 0; i < subSizes_.size() - 1; i++)
            {
                arrayRegions *= subSizes_[i];
            }
        }
        else
        {
            for (size_t i = subSizes_.size() - 1; i > 0; i--)
            {
                arrayRegions *= subSizes_[i];
            }
        }
    }
    return arrayRegions;
}

size_t SubarrayDataType::getArrayMemoryLocation(size_t region) const
{
    size_t arrayOffset = 0;

    // Construct the indices into each dimension for this region
    // Note: that the first dimension isn't used for column major, and the last
    // dimension isn't used for row-major.  It seems more readable this way.
    vector<size_t> dimIdxs(subSizes_.size(), 0);
    if (C_ORDER == order_)
    {
        // Make one pass to determine what indices correspond with this region
        size_t quotient = region;
        for (size_t i = dimIdxs.size() - 2; i > size_t(-1); i--)
        {
            cerr << quotient << " " << sizes_[i] << " " << endl;

            dimIdxs[i] = quotient % sizes_[i];
            quotient /= sizes_[i];
        }

        // Use the dimension indices to construct the offset
        for (size_t i = 0; i < dimIdxs.size() - 1; i++)
        {
            arrayOffset = (starts_[i] + dimIdxs[i]) + sizes_[i] * arrayOffset;
        }
    }
    else
    {
        // Make one pass to determine what indices correspond with this region
        size_t quotient = region;
        for (size_t i = 1; i < dimIdxs.size(); i++)
        {
            dimIdxs[i] = quotient % sizes_[i];
            quotient /= sizes_[i];
        }

        // Use the dimension indices to construct the offset
        for (size_t i = dimIdxs.size() - 1; i > 0; i--)
        {
            arrayOffset = (starts_[i] + dimIdxs[i]) + sizes_[i] * arrayOffset;
        }
    }
    return arrayOffset;
}

size_t SubarrayDataType::getSubarrayContiguousCount() const
{
    size_t contiguousCount = 0;
    if (subSizes_.size() > 0)
    {
        if (C_ORDER == order_)
        {
            contiguousCount = subSizes_[subSizes_.size() - 1];
        }
        else
        {
            contiguousCount = subSizes_[0];
        }
    }
    return contiguousCount;
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
