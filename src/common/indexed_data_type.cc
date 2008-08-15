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
    FSOffset typeBegin = (byteOffset / getTrueExtent()) * getTrueExtent();
    FSOffset currentOffset = byteOffset;
    while (bytesProcessed < numBytes)
    {
        // Begin processing the struct subtypes
        for (size_t i = 0;
             i < blockLengths_.size() && bytesProcessed < numBytes;
             i++)
        {
            size_t typeOffset = currentOffset % getTrueExtent();
            size_t displacement = displacements_[i] * oldType_.getTrueExtent();
            if (typeOffset > displacement &&
                typeOffset < (displacement +
                              blockLengths_[i] * oldType_.getTrueExtent()))
            {
                // Need to perform processing for only a portion of this type
                FSSize offsetDiff = typeOffset - displacement;
                FSSize dataLength =
                    min(blockLengths_[i] *oldType_.getTrueExtent() - offsetDiff,
                        numBytes - bytesProcessed);

                vector<FileRegion> elementRegions =
                    oldType_.getRegionsByBytes(currentOffset, dataLength);

                // Add regions to the total regions vector
                copy(elementRegions.begin(),
                     elementRegions.end(),
                     back_inserter(vectorRegions));

                // Update the number of bytes processed
                bytesProcessed += dataLength;
            }
            else if (typeOffset <= displacement)
            {
                FSSize dataLength =
                    min(blockLengths_[i] * oldType_.getTrueExtent(),
                        numBytes - bytesProcessed);
                vector<FileRegion> elementRegions =
                    oldType_.getRegionsByBytes(typeBegin + displacement,
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
        typeBegin += getTrueExtent();
        currentOffset = typeBegin;
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
