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
using namespace std;


int DataTypeProcessor::createFileLayoutForClient(
    const FSOffset& offset,
    const FSDataType& dataType,
    const size_t& count,
    const FileDistribution& dist,
    const size_t& maxBytesToProcess,
    DataTypeLayout& layout)
{
    size_t bytesProcessed = processClientRequest(offset,
                                                 dataType,
                                                 count,
                                                 dist,
                                                 maxBytesToProcess,
                                                 layout);
    assert(0 <= bytesProcessed);
    assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return bytesProcessed;
}

int DataTypeProcessor::createFileLayoutForServer(
    const FSOffset& offset,
    const FSDataType& dataType,
    const size_t& count,
    const FileDistribution& dist,
    const size_t& maxBytesToProcess,
    DataTypeLayout& layout)
{
    size_t bytesProcessed = processServerRequest(offset,
                                                 dataType,
                                                 count,
                                                 dist,
                                                 maxBytesToProcess,
                                                 layout);
    assert(0 <= bytesProcessed);
    assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return bytesProcessed;    
}



int DataTypeProcessor::processClientRequest(
    const FSOffset& offset,
    const FSDataType& dataType,
    const std::size_t& count,
    const FileDistribution& dist,
    const std::size_t& maxBytesToProcess,
    DataTypeLayout& layout)
{
    int bytesProcessed = 0;
    bytesProcessed = processContiguousRegion(offset,
                                             dataType*count,
                                             dist,
                                             maxBytesToProcess,
                                             layout);
    return bytesProcessed;
}

int DataTypeProcessor::processServerRequest(
    const FSOffset& offset,
    const FSDataType& dataType,
    const std::size_t& count,
    const FileDistribution& dist,
    const std::size_t& maxBytesToProcess,
    DataTypeLayout& layout)
{
    int bytesProcessed = 0;
    bytesProcessed = processContiguousRegion(offset,
                                             dataType*count,
                                             dist,
                                             maxBytesToProcess,
                                             layout);
    return bytesProcessed;
}

int DataTypeProcessor::processContiguousRegion(
    const FSOffset& offset,
    const FSSize& extent,
    const FileDistribution& dist,
    const std::size_t& maxBytesToProcess,
    DataTypeLayout& layout)
{
    // Determine the first mapped offset for this server
    FSOffset logServerOffset = dist.nextMappedLogicalOffset(offset);

    // Verify that offset is within requested region
    while (logServerOffset < (offset + extent))
    {
        // Determine the contiguous length forward from the physical offset
        FSOffset physOffset = dist.logicalToPhysicalOffset(logServerOffset);
        FSSize serverExtent = dist.contiguousLength(physOffset);

        // Determine how much of the extent to actually use
        FSSize requestedExtent = min(serverExtent, extent);
        if (0 != maxBytesToProcess)
        {
            requestedExtent = min(requestedExtent, maxBytesToProcess);
        }
        
        // Add region to the layout
        layout.addRegion(logServerOffset, requestedExtent);

        // Determine the next mapped offset for this server
        logServerOffset = dist.nextMappedLogicalOffset(logServerOffset +
                                                       serverExtent);
    }

    return extent;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
