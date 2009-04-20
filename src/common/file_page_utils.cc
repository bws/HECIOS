//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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
#include "file_page_utils.h"
#include <algorithm>
#include <cassert>
#include "basic_data_type.h"
#include "block_indexed_data_type.h"
#include "data_type_layout.h"
#include "data_type_processor.h"
#include "file_distribution.h"
#include "file_view.h"
using namespace std;

FilePageUtils::FilePageUtils()
{

}

FilePageUtils::~FilePageUtils()
{

}

FSOffset FilePageUtils::pageBeginOffset(const FSSize& pageSize,
                                        const FilePageId& pageId) const
{
    return (pageId * pageSize);
}

set<FilePageId> FilePageUtils::determineRequestPages(const FSSize& pageSize,
                                                     const FSOffset& offset,
                                                     const FSSize& size,
                                                     const FileView& view) const
{
    // Flatten view into file regions for the correct size
    vector<FileRegion> requestRegions =
        DataTypeProcessor::locateFileRegions(offset, size, view);

    // Convert regions into file pages
    return regionsToPageIds(pageSize, requestRegions);
}

set<FilePageId> FilePageUtils::determineRequestPages(const FSSize& pageSize,
                                                     const FSOffset& offset,
                                                     const FSSize& size,
                                                     const FileView& view,
                                                     const FileDistribution& dist) const
{
    // Flatten view into the physical file regions
    DataTypeLayout serverLayout;
    DataTypeProcessor::createServerFileLayoutForWrite(offset, size, view, dist,
                                                      serverLayout);

    // Convert the physical file regions into logical file regions
    vector<FileRegion> logicalRegions;
    vector<FileRegion> physicalRegions = serverLayout.getRegions();
    for (size_t i = 0; i < physicalRegions.size(); i++)
    {
        FSOffset currentOffset = dist.physicalToLogicalOffset(physicalRegions[i].offset);
        FSSize extent = physicalRegions[i].extent;

        FileRegion logicalRegion = {currentOffset, extent};
        logicalRegions.push_back(logicalRegion);
    }

    return regionsToPageIds(pageSize, logicalRegions);
}

set<FilePageId> FilePageUtils::determineRequestFullPages(const FSSize& pageSize,
                                                         const FSOffset& offset,
                                                         const FSSize& size,
                                                         const FileView& view) const
{
    set<FilePageId> allPageIds = determineRequestPages(pageSize, offset, size, view);
    set<FilePageId> partialPageIds = determineRequestPartialPages(pageSize, offset, size, view);
    set<FilePageId> fullPageIds;

    // Do this the easy way for now, subtract the partial pages from the
    // full pages.  If its too slow, this can be optimized later
    set_difference(allPageIds.begin(), allPageIds.end(),
                   partialPageIds.begin(), partialPageIds.end(),
                   inserter(fullPageIds, fullPageIds.begin()));
    return fullPageIds;
}

set<FilePageId> FilePageUtils::determineRequestPartialPages(const FSSize& pageSize,
                                                            const FSOffset& offset,
                                                            const FSSize& size,
                                                            const FileView& view) const
{
    set<FilePageId> partialPageIds;

    // Flatten view into file regions for the correct size
    vector<FileRegion> requestRegions =
        DataTypeProcessor::locateFileRegions(offset, size, view);

    for (size_t i = 0; i < requestRegions.size(); i++)
    {
        // For each contiguous region, only need to check if the first page
        // begins on a boundary and if the last page ends on a boundary.
        // Duplicate insertion is not a problems since we are using the
        // stl::set datatype
        FSOffset begin = requestRegions[i].offset;
        if (0 != (begin % pageSize))
        {
            size_t pageId = begin / pageSize;
            partialPageIds.insert(pageId);
        }

        FSOffset end = begin + requestRegions[i].extent;
        if (0 != (end % pageSize))
        {
            size_t pageId = end / pageSize;
            partialPageIds.insert(pageId);
        }
    }
    return partialPageIds;
}

FileRegionSet FilePageUtils::determinePartialPageRegions(const FSSize& pageSize,
                                                         const FilePageId& pageId,
                                                         const FSOffset& offset,
                                                         const FSSize& size,
                                                         const FileView& view) const
{
    FileRegionSet pageRegions;

    // Flatten view into file regions for the correct size
    vector<FileRegion> requestRegions =
        DataTypeProcessor::locateFileRegions(offset, size, view);

    FSOffset pageBegin = pageId * pageSize;
    FSOffset pageEnd = pageBegin + pageSize;
    for (size_t i = 0; i < requestRegions.size(); i++)
    {
        //cerr << "Next Page: " << pageId << " paginate reg: " << requestRegions[i] << endl;

        // The beginning and/or the end of the region must reside on the
        // requested page in order for it to be partial to the page
        // Find the regions for this page, truncate parts not on the page
        // and add them to the list
        FSOffset regionBegin = requestRegions[i].offset;
        FSOffset regionEnd = regionBegin + requestRegions[i].extent;
        if (regionBegin >= pageBegin && regionBegin < pageEnd)
        {
            // This region starts at offset and continues to the minimum
            // of the extent or the page end;
            FileRegion partial;
            partial.offset = regionBegin;
            partial.extent = min(pageEnd, regionEnd) - partial.offset;
            //cerr << "Begin Partial region result: " << partial << endl;
            pageRegions.insert(partial);
        }
        else if (regionEnd >= pageBegin && regionEnd < pageEnd)
        {
            // This region starts at page begin and continues to the end
            // of the extent
            FileRegion partial;
            partial.offset = pageBegin;
            partial.extent = regionEnd - partial.offset;
            //cerr << "End Partial region result: " << partial << endl;
            pageRegions.insert(partial);
        }
        else
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Region: " << requestRegions[i]
                 << " does not have a partial region on page: " << pageId << endl;
            assert(0);
        }
    }
    return pageRegions;
}

set<FilePageId> FilePageUtils::regionsToPageIds(const FSSize& pageSize,
                                                const vector<FileRegion>& fileRegions) const
{
    set<FilePageId> spanningPageIds;
    for (size_t i = 0; i < fileRegions.size(); i++)
    {
        // Determine the first and last page
        FSOffset beginOffset = fileRegions[i].offset;
        FSOffset endOffset = beginOffset + fileRegions[i].extent - 1;
        size_t firstPage = beginOffset / pageSize;
        size_t lastPage = endOffset / pageSize;

        for (size_t j = firstPage; j <= lastPage; j++)
        {
            FilePageId id = j;
            spanningPageIds.insert(id);
        }
    }
    return spanningPageIds;
}

set<FilePage> FilePageUtils::regionsToPages(const FSSize& pageSize,
                                            const vector<FileRegion>& fileRegions) const
{
    set<FilePage> spanningPages;
    set<FilePageId> spanningIds = regionsToPageIds(pageSize, fileRegions);
    set<FilePageId>::const_iterator idIter;
    set<FilePageId>::const_iterator idEnd = spanningIds.end();
    for (idIter = spanningIds.begin(); idIter != idEnd; ++idIter)
    {
        FilePage fp(pageBeginOffset(pageSize, *idIter), pageSize);
        spanningPages.insert(fp);
    }
    return spanningPages;
}

FileView* FilePageUtils::createPageViewDescriptor(const FSSize& pageSize,
                                                 const set<FilePageId>& pageIds) const
{
    assert(!pageIds.empty());

    // Create the vector of displacements
    set<FilePageId>::const_iterator idIter = pageIds.begin();
    vector<size_t> displacements(pageIds.size());
    for (size_t i = 0; i < displacements.size(); i++)
    {
        displacements[i] = (*idIter) * pageSize;
        ++idIter;
    }

    // Create the block indexed data type to use as the view
    static ByteDataType byteDataType;
    BlockIndexedDataType* viewType = new BlockIndexedDataType(pageSize,
                                                              displacements,
                                                              byteDataType);
    return (new FileView(0, viewType));
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
