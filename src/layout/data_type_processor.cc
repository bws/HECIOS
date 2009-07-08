//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "data_type_processor.h"
#include <cassert>
#include "data_type_layout.h"
#include "file_distribution.h"
#include "file_view.h"
using namespace std;


FSSize DataTypeProcessor::createClientFileLayoutForRead(
    const FSOffset& offset,
    const DataType& dataType,
    const size_t& count,
    const FileView& view,
    const FileDistribution& dist,
    const FSSize& bstreamSize,
    FSSize& outAggregateSize)
{
    FSSize assignedBytes = processClientRequest(offset,
                                                dataType,
                                                count,
                                                view,
                                                dist,
                                                bstreamSize,
                                                false,
                                                outAggregateSize);
    assert(0 <= assignedBytes);
    assert(0 <= outAggregateSize);
    //assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return assignedBytes;
}

FSSize DataTypeProcessor::createClientFileLayoutForWrite(
    const FSOffset& offset,
    const DataType& dataType,
    const size_t& count,
    const FileView& view,
    const FileDistribution& dist,
    FSSize& outAggregateSize)
{
    FSSize assignedBytes = processClientRequest(offset,
                                                dataType,
                                                count,
                                                view,
                                                dist,
                                                0,
                                                true,
                                                outAggregateSize);
    assert(0 <= assignedBytes);
    assert(0 <= outAggregateSize);
    //assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return assignedBytes;
}

FSSize DataTypeProcessor::createServerFileLayoutForRead(
    const FSOffset& offset,
    const FSSize& dataSize,
    const FileView& view,
    const FileDistribution& dist,
    const FSSize& bstreamSize,
    DataTypeLayout& outLayout)
{
    FSSize bytesProcessed = processServerRequest(offset,
                                                 dataSize,
                                                 view,
                                                 dist,
                                                 bstreamSize,
                                                 false,
                                                 outLayout);
    assert(0 <= bytesProcessed);
    //assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return bytesProcessed;
}

FSSize DataTypeProcessor::createServerFileLayoutForWrite(
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
                                                 0,
                                                 true,
                                                 outLayout);
    assert(0 <= bytesProcessed);
    //assert(bytesProcessed <= maxBytesToProcess || 0 == maxBytesToProcess);
    return bytesProcessed;
}

vector<FileRegion> DataTypeProcessor::locateFileRegions(const FSOffset& offset,
                                                        const FSSize& dataSize,
                                                        const FileView& view)
{
    // Determine the amount of contiguous file regions that correspond to
    // this server's physical file locations
    FSSize disp = view.getDisplacement();
    const DataType* fileDataType = view.getDataType();
    vector<FileRegion> fileRegions = fileDataType->getRegionsByBytes(offset,
                                                                     dataSize);

    // Modify the file regions to take into account the view displacement
    for (size_t i = 0; i < fileRegions.size(); i++)
    {
        fileRegions[i]. offset += disp;
    }
    return fileRegions;
}

FileRegionSet DataTypeProcessor::locateFileRegionSet(const FSOffset& offset,
                                                     const FSSize& dataSize,
                                                     const FileView& view)
{
    // Determine the amount of contiguous file regions that correspond to
    // this server's physical file locations
    FSSize disp = view.getDisplacement();
    const DataType* fileDataType = view.getDataType();
    vector<FileRegion> fileRegions = fileDataType->getRegionsByBytes(offset,
                                                                     dataSize);

    // Modify the file regions to take into account the view displacement
    // and insert into a file region set
    FileRegionSet frs;
    for (size_t i = 0; i < fileRegions.size(); i++)
    {
        fileRegions[i].offset += disp;
        frs.insert(fileRegions[i]);
    }
    return frs;
}

FSSize DataTypeProcessor::processClientRequest(
    const FSOffset& offset,
    const DataType& dataType,
    const std::size_t& count,
    const FileView& view,
    const FileDistribution& dist,
    const FSSize& bstreamSize,
    bool dataExtend,
    FSSize& outAggregateSize)
{
    // It isn't necessary to flatten the memory data type, simply figure out
    // its magnitude
    outAggregateSize = dataType.getExtent() * count;

    // Determine the offsets and extents into the memory buffer to send to
    // the server described in the distribution
    //
    // For the simulator it isn't necessary to get the actual offsets, so
    // use the server's results to construct the size of each server's
    // data receipt
    DataTypeLayout layout;
    return processServerRequest(offset, outAggregateSize, view, dist, bstreamSize, dataExtend, layout);
}

FSSize DataTypeProcessor::processServerRequest(const FSOffset& offset,
                                               const FSSize& dataSize,
                                               const FileView& view,
                                               const FileDistribution& dist,
                                               const FSSize& bstreamSize,
                                               bool dataExtend,
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
                                   bstreamSize,
                                   dataExtend,
                                   outLayout);
    }

    FSSize length = outLayout.getLength();
    return length;
}

void DataTypeProcessor::distributeContiguousRegion(
    const FSOffset& offset,
    const FSSize& extent,
    const FileDistribution& dist,
    const FSSize& bstreamSize,
    bool dataExtend,
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

        // If the dataExtend flag is set add the region
        // Otherwise, trim region to include only existing data
        if (dataExtend)
        {
            // Add region to the layout
            outLayout.addRegion(physOffset, requestedExtent);
        }
        else if (FSSize(physOffset) < bstreamSize)
        {
            // Choose the min of request extent or stream extent
            FSOffset finalSize = min(physOffset + requestedExtent, bstreamSize);

            // Add region to layout
            outLayout.addRegion(physOffset, finalSize - physOffset);
        }

        // Determine the next mapped offset for this server
        logServerOffset = dist.nextMappedLogicalOffset(logServerOffset +
                                                       serverExtent);
    }
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
