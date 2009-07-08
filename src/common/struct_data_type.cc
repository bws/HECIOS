//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include "struct_data_type.h"
using namespace std;

size_t StructDataType::calculateExtent(vector<size_t> blockLengths,
                                       vector<size_t> displacements,
                                       vector<const DataType*> oldTypes)
{
    size_t extent = 0;
    for (size_t i = 0; i < blockLengths.size(); i++)
    {
        size_t currentRange = displacements[i] +
            (blockLengths[i] * oldTypes[i]->getExtent());
        extent = max(extent, currentRange);
    }
    return extent;
}

StructDataType::StructDataType(vector<size_t> blockLengths,
                               vector<size_t> displacements,
                               vector<const DataType*> oldTypes) :
    DataType(StructDataType::calculateExtent(blockLengths, displacements,
                                             oldTypes)),
    blockLengths_(blockLengths),
    displacements_(displacements),
    types_(oldTypes)
{
    // Ensure the displacements are in increasing order
    for (size_t i = 1; i < displacements_.size(); i++)
    {
        assert(displacements_[i-1] <= displacements_[i]);
    }
}

StructDataType::StructDataType(const StructDataType& other)
    : DataType(other),
    blockLengths_(other.blockLengths_),
    displacements_(other.displacements_),
    types_(other.types_)
{
}

StructDataType::~StructDataType()
{

}

StructDataType* StructDataType::clone() const
{
    return new StructDataType(*this);
}

size_t StructDataType::getRepresentationByteLength() const
{
    size_t length = 0;
    for (size_t i = 0; i < types_.size(); i++)
    {
        length += 4 + 4 + types_[i]->getRepresentationByteLength();
    }
    return length;
}

vector<FileRegion> StructDataType::getRegionsByBytes(const FSOffset& byteOffset,
                                                     size_t numBytes) const
{
    // The total regions produced
    vector<FileRegion> vectorRegions;

    // Construct the regions required to map numBytes of data to data regions
    size_t bytesProcessed = 0;
    FSOffset structBegin = (byteOffset / getTrueExtent()) * getTrueExtent();
    FSOffset currentOffset = byteOffset;
    while (bytesProcessed < numBytes)
    {
        // Begin processing the struct subtypes
        for (size_t i = 0; i < types_.size() && bytesProcessed < numBytes; i++)
        {
            size_t structOffset = currentOffset % getTrueExtent();
            if (structOffset > displacements_[i] &&
                structOffset < (displacements_[i] +
                                (blockLengths_[i] * types_[i]->getTrueExtent())))
            {
                // Need to perform processing for only a portion of this type
                FSSize offsetDiff = structOffset - displacements_[i];
                FSSize dataLength =
                    min(blockLengths_[i] * types_[i]->getExtent() - offsetDiff,
                        numBytes - bytesProcessed);

                vector<FileRegion> elementRegions =
                    types_[i]->getRegionsByBytes(currentOffset, dataLength);

                // Add regions to the total regions vector
                copy(elementRegions.begin(),
                     elementRegions.end(),
                     back_inserter(vectorRegions));

                // Update the number of bytes processed
                bytesProcessed += dataLength;
            }
            else if (structOffset <= displacements_[i])
            {
                FSSize dataLength = min(blockLengths_[i] * types_[i]->getExtent(),
                                        numBytes - bytesProcessed);
                vector<FileRegion> elementRegions =
                    types_[i]->getRegionsByBytes(structBegin + displacements_[i],
                                                 dataLength);

                // Add regions to the total regions vector
                copy(elementRegions.begin(),
                     elementRegions.end(),
                     back_inserter(vectorRegions));

                // Update the number of bytes processed
                bytesProcessed += dataLength;
            }
        }

        // Increment to the next struct
        structBegin += getTrueExtent();
        currentOffset = structBegin;
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
