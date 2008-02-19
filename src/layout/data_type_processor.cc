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

#include "data_type_processor.h"
#include <cassert>
#include "data_type_layout.h"
#include "file_distribution.h"
#include "file_view.h"
using namespace std;


FSSize DataTypeProcessor::createFileLayoutForClient(
    const FSOffset& offset,
    const DataType& dataType,
    const size_t& count,
    const FileView& view,
    const FileDistribution& dist,
    FSSize& outAggregateSize)
{
    FSSize bytesProcessed = processClientRequest(offset,
                                                 dataType,
                                                 count,
                                                 view,
                                                 dist,
                                                 outAggregateSize);
    assert(0 <= bytesProcessed);
    assert(0 <= outAggregateSize);
    //assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return bytesProcessed;
}

FSSize DataTypeProcessor::createFileLayoutForServer(
    const FSOffset& offset,
    const FSSize& dataSize,
    const FileView& view,
    const FileDistribution& dist,
    DataTypeLayout& outLayout)
{
    FSSize bytesProcessed = processServerRequest(offset,
                                                 dataSize,
                                                 view,
                                                 dist,
                                                 outLayout);
    assert(0 <= bytesProcessed);
    //assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return bytesProcessed;    
}



FSSize DataTypeProcessor::processClientRequest(
    const FSOffset& offset,
    const DataType& dataType,
    const std::size_t& count,
    const FileView& view,
    const FileDistribution& dist,
    FSSize& outAggregateSize)
{
    // It isn't neccesary to flatten the memory data type, simply figure out
    // its magnitude
    outAggregateSize = dataType.getExtent() * count;

    // Determine the offsets and extents into the memory buffer to send to
    // the server described in the distribution
    //
    // For the simulator it isn't neccesary to get the actual offsets, so
    // use the server's results to construct the size of each server's
    // data receipt
    DataTypeLayout layout;
    return processServerRequest(offset, outAggregateSize, view, dist, layout);
}

FSSize DataTypeProcessor::processServerRequest(const FSOffset& offset,
                                               const FSSize& dataSize,
                                               const FileView& view,
                                               const FileDistribution& dist,
                                               DataTypeLayout& outLayout)
{
    // Determine the amount of contiguous file regions that correspond to
    // this server's physical file locations
    FSSize disp = view.getDisplacement();
    const DataType* fileDataType = view.getDataType();
    vector<FileRegion> fileRegions = fileDataType->getRegionsByBytes(offset,
                                                                     dataSize);
    for (size_t i = 0; i < fileRegions.size(); i++)
    {
        // Construct the data layout for this server distribution
        distributeContiguousRegion(disp + fileRegions[i].offset,
                                   fileRegions[i].extent,
                                   dist,
                                   outLayout);
    }

    FSSize length = outLayout.getLength();
    return length;
}

void DataTypeProcessor::distributeContiguousRegion(
    const FSOffset& offset,
    const FSSize& extent,
    const FileDistribution& dist,
    DataTypeLayout& outLayout)
{
    // Determine the first mapped offset for this server
    FSOffset logServerOffset = dist.nextMappedLogicalOffset(offset);

    // Verify that offset is within requested region
    while (FSSize(logServerOffset) < (offset + extent))
    {
        // Determine the contiguous length forward from the physical offset
        FSOffset physOffset = dist.logicalToPhysicalOffset(logServerOffset);
        FSSize serverExtent = dist.contiguousLength(physOffset);

        // Determine how much of the extent to actually use
        FSSize requestedExtent = min(serverExtent, extent);
        
        // Add region to the layout
        outLayout.addRegion(physOffset, requestedExtent);

        // Determine the next mapped offset for this server
        logServerOffset = dist.nextMappedLogicalOffset(logServerOffset +
                                                       serverExtent);
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
