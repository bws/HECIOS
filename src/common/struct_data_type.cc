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
#include "struct_data_type.h"
using namespace std;

size_t StructDataType::calculateExtent(vector<size_t> blockLengths,
                                       vector<size_t> displacements,
                                       vector<DataType*> oldTypes)
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
                               vector<DataType*> oldTypes) :
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
    FSOffset currentOffset = byteOffset;
    while (bytesProcessed < numBytes)
    {
        for (size_t i = 0; i < types_.size() && bytesProcessed < numBytes; i++)
        {
            FSSize dataLength = min(blockLengths_[i] * types_[i]->getExtent(),
                                numBytes - bytesProcessed);
            vector<FileRegion> elementRegions =
                types_[i]->getRegionsByBytes(currentOffset + displacements_[i],
                                             dataLength);

            // Add regions to the total regions vector
            copy(elementRegions.begin(),
                 elementRegions.end(),
                 back_inserter(vectorRegions));

            // Update the number of bytes processed
            bytesProcessed += dataLength;
        }
        currentOffset += getExtent();
    }
    assert(bytesProcessed == numBytes);
    return vectorRegions;
}

vector<FileRegion> StructDataType::getRegionsByCount(const FSOffset& byteOffset,
                                                     size_t count) const
{
    // The total regions produced for count vectors
    vector<FileRegion> vectorRegions;

    // Flatten the types in order to construct count of the new type
    for (size_t i = 0; i < count; i++)
    {
        FSOffset countOffset = i * getExtent();
        for (size_t j = 0; j < types_.size(); j++)
        {
            // Get the regions for the next type in the struct
            FSOffset nextOffset = byteOffset + countOffset + displacements_[j];
            vector<FileRegion> elementRegions =
                types_[j]->getRegionsByCount(nextOffset, blockLengths_[j]);

            // Copy the regions to the output vector
            copy(elementRegions.begin(),
                 elementRegions.end(),
                 back_inserter(vectorRegions));
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
