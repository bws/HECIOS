//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include "subarray_data_type.h"
using namespace std;

size_t SubarrayDataType::calculateExtent(vector<size_t> sizes,
                                         const DataType& oldDataType)
{
    //cerr << "Old Type Ext: " << oldDataType.getTrueExtent() << endl;
    size_t extent = oldDataType.getTrueExtent();
    for (size_t i = 0; i < sizes.size(); i++)
    {
        extent *= sizes[i];
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
    // Validate that subarray specification is valid (p 162 of MPI-I)
    for (size_t i = 0; i < sizes_.size(); i++)
    {
        assert(1 <= subSizes_[i]);
        assert(0 <= starts_[i]);
        assert(subSizes[i] <= sizes_[i]);
        assert(starts_[i] <=  sizes_[i] - subSizes_[i]);
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
    assert(0 != getTrueExtent());

    //cerr << "Offset: " << byteOffset << " Size: " << numBytes << endl;
    vector<FileRegion> regions;

    // Determine the size of the contiguous data type region
    size_t contigCount = getSubarrayContiguousCount();
    size_t contigLength = contigCount * oldType_.getTrueExtent();
    //cerr << "Contig count: " << contigCount << " ContigLength: " << contigLength << endl;

    size_t bytesProcessed = 0;
    size_t subSizeExtent = getSubSizeExtent();
    //cerr << "ByteOff: " << byteOffset << " SubExt: " << subSizeExtent << endl;
    FSOffset fullArrayOffset = (byteOffset / subSizeExtent) * getTrueExtent();
    FSOffset subArrayOffset = byteOffset % subSizeExtent;
    while (bytesProcessed < numBytes)
    {
        size_t firstRegion = subArrayOffset / subSizeExtent;
        size_t numRegions = getNumArrayRegions();
        //cerr << "Num Regions: " << numRegions << " begin: " << firstRegion << endl;

        // Process regions in the array until enough bytes are processed
        // or the array
        for (size_t i = firstRegion; (i < numRegions) && (bytesProcessed < numBytes); i++)
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
            dataLength = min(dataLength, numBytes - bytesProcessed);

            // Get region data
            vector<FileRegion> elementRegions =
                oldType_.getRegionsByBytes(fullArrayOffset + regionOffset,
                                           dataLength);

            // Add regions to the total regions vector
            copy(elementRegions.begin(),
                 elementRegions.end(),
                 back_inserter(regions));

            // Update the number of bytes processed
            bytesProcessed += dataLength;
            //cerr << "bytes: " << dataLength << " total: " << bytesProcessed << endl;
        }

        // Increment the array offset to the next subarray
        //cerr << "Array offset: " << fullArrayOffset << " TrueExt: " << getTrueExtent();
        fullArrayOffset += getTrueExtent();
        subArrayOffset = 0;
        //cerr << " New array offset: " << fullArrayOffset << endl;
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
    vector<size_t> dimIdxs(subSizes_.size(), 0);
    if (C_ORDER == order_)
    {
        // Make one pass to determine what indices correspond with this region
        size_t quotient = region;
        for (size_t i = dimIdxs.size() - 2; i != size_t(-1); i--)
        {
            dimIdxs[i] = quotient % subSizes_[i];
            quotient /= subSizes_[i];
        }

        // Use the dimension indices to construct the offset
        for (size_t i = 0; i < dimIdxs.size(); i++)
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
            dimIdxs[i] = quotient % subSizes_[i];
            quotient /= subSizes_[i];
        }

        // Use the dimension indices to construct the offset
        for (size_t i = dimIdxs.size() - 1; i != size_t(-1); i--)
        {
            arrayOffset = (starts_[i] + dimIdxs[i]) + sizes_[i] * arrayOffset;
        }
    }

    // Modify the array offset to take into account the element size
    arrayOffset *= oldType_.getTrueExtent();

    //cerr << "Memory Loc: " << region << " --> ";
    //for (size_t i = 0; i < dimIdxs.size(); i++)
    //{
    //    cerr << "[" << starts_[i] + dimIdxs[i] << "]";
    //}
    //cerr << " --> " << arrayOffset << endl;
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

size_t SubarrayDataType::getArrayContiguousCount() const
{
    size_t contiguousCount = 0;
    if (sizes_.size() > 0)
    {
        if (C_ORDER == order_)
        {
            contiguousCount = sizes_[sizes_.size() - 1];
        }
        else
        {
            contiguousCount = sizes_[0];
        }
    }
    return contiguousCount;
}

size_t SubarrayDataType::getSubSizeExtent() const
{
    size_t extent = oldType_.getTrueExtent();
    for (size_t i = 0; i < subSizes_.size(); i++)
    {
        extent *= subSizes_[i];
    }
    return extent;
}

ostream& SubarrayDataType::print(ostream& ost) const
{
    ost << "Subarray Sizes[";
    for (size_t i = 0; i < sizes_.size(); i++)
    {
        if (i != 0)
            ost << ",";
        ost << sizes_[i];
    }

    ost << "] SubSizes[";
    for (size_t i = 0; i < subSizes_.size(); i++)
    {
        if (i != 0)
            ost << ",";
        ost << subSizes_[i];
    }

    ost << "] Starts [";
    for (size_t i = 0; i < starts_.size(); i++)
    {
        if (i != 0)
            ost << ",";
        ost << starts_[i];
    }

    if (C_ORDER == order_)
    {
        ost << "] C Order ";
    }
    else
    {
        ost << "] Fortran Order ";
    }
    ost << "OldType: " << oldType_;
    return ost;
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
