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
#include "file_distribution.h"
using namespace std;


int DataTypeProcessor::createFileLayoutForClient(
    const FSOffset& offset,
    const FSDataType& dataType,
    const size_t& count,
    const FileDistribution& dist,
    const size_t& maxBytesToProcess,
    FileLayout& layout)
{
    size_t bytesProcessed = processClientRequest(offset,
                                                 dataType,
                                                 count,
                                                 dist,
                                                 maxBytesToProcess,
                                                 layout);
    assert(0 <= bytesProcessed && bytesProcessed <= maxBytesToProcess);
    return bytesProcessed;
}

int DataTypeProcessor::createFileLayoutForServer(
    const FSOffset& offset,
    const FSDataType& dataType,
    const size_t& count,
    const FileDistribution& dist,
    const size_t& maxBytesToProcess,
    FileLayout& layout)
{
    size_t bytesProcessed = processServerRequest(offset,
                                                 dataType,
                                                 count,
                                                 dist,
                                                 maxBytesToProcess,
                                                 layout);
    assert(0 <= bytesProcessed && bytesProcessed <= maxBytesToProcess);
    return bytesProcessed;    
}



int DataTypeProcessor::processClientRequest(
    const FSOffset& offset,
    const FSDataType& dataType,
    const std::size_t& count,
    const FileDistribution& dist,
    const std::size_t& maxBytesToProcess,
    FileLayout& layout)
{
    int bytesProcessed = 0;
    bytesProcessed = processContiguousServerRegion(offset,
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
    FileLayout& layout)
{
    int bytesProcessed = 0;
    bytesProcessed = processContiguousServerRegion(offset,
                                                   dataType*count,
                                                   dist,
                                                   maxBytesToProcess,
                                                   layout);
    return bytesProcessed;
}

int DataTypeProcessor::processContiguousClientRegion(
    const FSOffset& offset,
    const FSSize& extent,
    const FileDistribution& dist,
    const std::size_t& maxBytesToProcess,
    FileLayout& layout)
{
    // Determine the first mapped offset for this server
    FSOffset logServerOffset = dist.nextMappedLogicalOffset(offset);

    // Verify that offset is within requested region
    while (logServerOffset < (offset + extent))
    {
        // Determine the contiguous length forward from the physical offset
        FSOffset physOffset = dist.logicalToPhysicalOffset(logServerOffset);
        FSSize serverExtent = dist.contiguousLength(physOffset);

        // Add region to the client's layout
        layout.offsets.push_back(logServerOffset);
        layout.extents.push_back(serverExtent);

        // Determine the next mapped offset for this server
        logServerOffset = dist.nextMappedLogicalOffset(logServerOffset +
                                                       serverExtent);
    }

    return extent;
}

int DataTypeProcessor::processContiguousServerRegion(
    const FSOffset& offset,
    const FSSize& extent,
    const FileDistribution& dist,
    const std::size_t& maxBytesToProcess,
    FileLayout& layout)
{
    // Determine the first mapped offset for this server
    FSOffset logServerOffset = dist.nextMappedLogicalOffset(offset);

    // Verify that offset is within requested region
    while (logServerOffset < (offset + extent))
    {
        // Determine the contiguous length forward from the physical offset
        FSOffset physOffset = dist.logicalToPhysicalOffset(logServerOffset);
        FSSize serverExtent = dist.contiguousLength(physOffset);

        // Add region to the server's file layout
        layout.offsets.push_back(physOffset);
        layout.extents.push_back(serverExtent);

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
