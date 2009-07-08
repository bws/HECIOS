//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include "indexed_data_type.h"
using namespace std;

size_t IndexedDataType::calculateExtent(const vector<size_t>& blockLengths,
                                        const vector<size_t>& displacements,
                                        const DataType& oldDataType)
{
    assert(blockLengths.size() == displacements.size());
    size_t lastIdx = blockLengths.size() - 1;
    size_t extent = (blockLengths[lastIdx] + displacements[lastIdx])
        * oldDataType.getTrueExtent();
    return extent;
}

IndexedDataType::IndexedDataType(const vector<size_t>& blockLengths,
                                 const vector<size_t>& displacements,
                                 const DataType& oldDataType)
    : DataType(calculateExtent(blockLengths, displacements, oldDataType)),
      blockLengths_(blockLengths),
      displacements_(displacements),
      oldType_(oldDataType)
{
    assert(blockLengths_.size() == displacements_.size());
    for (size_t i = 1; i < displacements_.size(); i++)
    {
        assert(displacements_[i] > displacements_[i-1]);
    }
}

IndexedDataType::IndexedDataType(const IndexedDataType& other)
    : DataType(other),
      blockLengths_(other.blockLengths_),
      displacements_(other.displacements_),
      oldType_(other.oldType_)
{
}

IndexedDataType::~IndexedDataType()
{

}

IndexedDataType* IndexedDataType::clone() const
{
    return new IndexedDataType(*this);
}

size_t IndexedDataType::getRepresentationByteLength() const
{
    size_t length = 4 + (4 + 4) * count() +
        oldType_.getRepresentationByteLength();
    return length;
}

vector<FileRegion> IndexedDataType::getRegionsByBytes(
    const FSOffset& byteOffset,
    size_t numBytes) const
{
    // The total regions produced
    vector<FileRegion> vectorRegions;

    // Construct the regions required to map numBytes of data to data regions
    size_t bytesProcessed = 0;
    size_t typeDataSize = getDataSize();
    FSOffset typeBegin = (byteOffset / typeDataSize) * getExtent();
    FSOffset currentOffset = byteOffset % typeDataSize;
    while (bytesProcessed < numBytes)
    {
        // Begin processing the indexed blocks
        size_t elementsProcessedSize = 0;
        for (size_t i = 0;
             i < blockLengths_.size() && bytesProcessed < numBytes;
             i++)
        {
            size_t typeOffset = currentOffset % getTrueExtent();
            size_t nextElementsSize = blockLengths_[i] * oldType_.getTrueExtent();
            if (typeOffset > elementsProcessedSize &&
                typeOffset < (elementsProcessedSize + nextElementsSize))
            {
                // Calculate the offset into this element block
                FSOffset elementOffset = typeOffset - elementsProcessedSize;

                // Need to perform processing for only a portion of this type
                FSSize dataLength = min(nextElementsSize - elementOffset,
                                        numBytes - bytesProcessed);

               FSOffset beginOff = typeBegin +
                    (typeOffset - elementsProcessedSize) +
                    (displacements_[i] * oldType_.getTrueExtent());
                vector<FileRegion> elementRegions =
                    oldType_.getRegionsByBytes(beginOff,
                                               dataLength);

                // Add regions to the total regions vector
                copy(elementRegions.begin(),
                     elementRegions.end(),
                     back_inserter(vectorRegions));

                // Update the number of bytes processed
                bytesProcessed += dataLength;
            }
            else if (typeOffset <= (elementsProcessedSize + nextElementsSize))
            {
                FSOffset beginOff = typeBegin +
                    (displacements_[i] * oldType_.getTrueExtent());
                FSSize dataLength = min(nextElementsSize,
                                        numBytes - bytesProcessed);
                vector<FileRegion> elementRegions =
                    oldType_.getRegionsByBytes(beginOff,
                                               dataLength);

                // Add regions to the total regions vector
                copy(elementRegions.begin(),
                     elementRegions.end(),
                     back_inserter(vectorRegions));

                // Update the number of bytes processed
                bytesProcessed += dataLength;
            }

            // Update the bookkeeping for the type
            elementsProcessedSize += nextElementsSize;
        }

        // Increment to the next count
        typeBegin += getExtent();
        currentOffset = typeBegin;
    }
    assert(bytesProcessed == numBytes);
    return vectorRegions;
}

size_t IndexedDataType::getDataSize() const
{
    size_t dataSize = 0;
    size_t oldTypeSize = oldType_.getTrueExtent();
    for (size_t i = 0; i < blockLengths_.size(); i++)
    {
        dataSize += blockLengths_[i] * oldTypeSize;
    }
    return dataSize;
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
