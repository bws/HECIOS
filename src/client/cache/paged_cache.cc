//
// This file is part of Hecios
//
// Copyright (C) 2008 Bradley W. Settlemyer
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
#include "paged_cache.h"
#include <algorithm>
#include <cassert>
#include "block_indexed_data_type.h"
#include "data_type_processor.h"
#include "file_builder.h"
#include "file_descriptor.h"
#include "file_region_set.h"
#include "file_view.h"
#include "mpi_proto_m.h"
using namespace std;

set<PagedCache::Key> PagedCache::convertPagesToCacheKeys(const Filename& filename,
                                                         set<FilePageId> pageIds)
{
    set<PagedCache::Key> keys;
    set<FilePageId>::const_iterator iter = pageIds.begin();
    set<FilePageId>::const_iterator end = pageIds.end();
    while (iter != end)
    {
        PagedCache::Key key(filename, *iter);
        keys.insert(key);
        iter++;
    }
    return keys;
}

/** @return the Page Ids for filename in the set of cache keys */
set<FilePageId> PagedCache::convertCacheKeysToPages(const Filename& filename,
                                                    set<PagedCache::Key> keys)
{
    set<FilePageId> pages;
    set<PagedCache::Key>::const_iterator iter = keys.begin();
    set<PagedCache::Key>::const_iterator end = keys.end();
    while (iter != end)
    {
        if (filename == iter->filename)
        {
            pages.insert(iter->key);
        }
        iter++;
    }
    return pages;
}


PagedCache::PagedCache()
{
}

PagedCache::~PagedCache()
{
}

FSOffset PagedCache::pageBeginOffset(const FilePageId& pageId) const
{
    return (pageId * pageSize_);
}

set<FilePageId> PagedCache::determineRequestPages(const FSOffset& offset,
                                                  const FSSize& size,
                                                  const FileView& view) const
{
    // Flatten view into file regions for the correct size
    vector<FileRegion> requestRegions =
        DataTypeProcessor::locateFileRegions(offset, size, view);

    // Convert regions into file pages
    return regionsToPageIds(requestRegions);
}

set<FilePageId> PagedCache::determineRequestFullPages(const FSOffset& offset,
                                                      const FSSize& size,
                                                      const FileView& view) const
{
    set<FilePageId> allPageIds = determineRequestPages(offset, size, view);
    set<FilePageId> partialPageIds = determineRequestPartialPages(offset, size, view);
    set<FilePageId> fullPageIds;

    // Do this the easy way for now, subtract the partial pages from the
    // full pages.  If its too slow, this can be optimized later
    set_difference(allPageIds.begin(), allPageIds.end(),
                   partialPageIds.begin(), partialPageIds.end(),
                   inserter(fullPageIds, fullPageIds.begin()));
    return fullPageIds;
}

set<FilePageId> PagedCache::determineRequestPartialPages(const FSOffset& offset,
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
        if (0 != (begin % pageSize_))
        {
            size_t pageId = begin / pageSize_;
            partialPageIds.insert(pageId);
        }

        FSOffset end = begin + requestRegions[i].extent;
        if (0 != (end % pageSize_))
        {
            size_t pageId = end / pageSize_;
            partialPageIds.insert(pageId);
        }
    }
    return partialPageIds;
}

FileRegionSet* PagedCache::determinePartialPageRegions(const FilePageId& pageId,
                                                        const FSOffset& offset,
                                                        const FSSize& size,
                                                        const FileView& view) const
{
    FileRegionSet* pageRegions = new FileRegionSet();

    // Flatten view into file regions for the correct size
    vector<FileRegion> requestRegions =
        DataTypeProcessor::locateFileRegions(offset, size, view);

    FSOffset pageBegin = pageId * pageSize_;
    FSOffset pageEnd = pageBegin + pageSize_;
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
            pageRegions->insert(partial);
        }
        else if (regionEnd >= pageBegin && regionEnd < pageEnd)
        {
            // This region starts at page begin and continues to the end
            // of the extent
            FileRegion partial;
            partial.offset = pageBegin;
            partial.extent = regionEnd - partial.offset;
            //cerr << "End Partial region result: " << partial << endl;
            pageRegions->insert(partial);
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

void PagedCache::initialize()
{
    MiddlewareCache::initialize();
    pageSize_ = par("pageSize");
    pageCapacity_ = par("pageCapacity");
}

spfsMPIFileReadAtRequest* PagedCache::createPageReadRequest(
    const Filename& filename,
    const set<FilePageId>& pageIds,
    spfsMPIFileRequest* origRequest) const
{
    assert(!pageIds.empty());
    assert(0 != origRequest);

    // Construct a descriptor that views only the correct pages
    FileDescriptor* fd = getPageViewDescriptor(filename, pageIds);

    // Create the read request
    spfsMPIFileReadAtRequest* readRequest =
        new spfsMPIFileReadAtRequest("PagedCache Read Request",
                                     SPFS_MPI_FILE_READ_AT_REQUEST);
    readRequest->setContextPointer(origRequest);
    readRequest->setFileDes(fd);
    readRequest->setDataType(new ByteDataType());
    readRequest->setCount(pageIds.size() * pageSize_);
    readRequest->setOffset(0);
    return readRequest;
}

spfsMPIFileWriteAtRequest* PagedCache::createPageWriteRequest(
    const Filename& filename,
    const set<FilePageId>& pageIds,
    spfsMPIFileRequest* origRequest) const
{
    assert(!pageIds.empty());

    // Construct a descriptor that views only the correct pages
    FileDescriptor* fd = getPageViewDescriptor(filename, pageIds);

    // Create the read request
    spfsMPIFileWriteAtRequest* writeRequest =
        new spfsMPIFileWriteAtRequest("PagedCache Write Request",
                                      SPFS_MPI_FILE_WRITE_AT_REQUEST);
    writeRequest->setContextPointer(origRequest);
    writeRequest->setFileDes(fd);
    writeRequest->setDataType(new ByteDataType());
    writeRequest->setCount(pageIds.size() * pageSize_);
    writeRequest->setOffset(0);
    return writeRequest;
}

set<FilePageId> PagedCache::regionsToPageIds(const vector<FileRegion>& fileRegions) const
{
    set<FilePageId> spanningPageIds;
    for (size_t i = 0; i < fileRegions.size(); i++)
    {
        // Determine the first and last page
        FSOffset begin = fileRegions[i].offset;
        FSOffset end = begin + fileRegions[i].extent;
        size_t firstPage = begin / pageSize_;
        size_t lastPage = end / pageSize_;
        for (size_t j = firstPage; j < lastPage; j++)
        {
            FilePageId id = j;
            spanningPageIds.insert(id);
        }
    }
    return spanningPageIds;
}

set<FilePage> PagedCache::regionsToPages(const vector<FileRegion>& fileRegions) const
{
    set<FilePage> spanningPages;
    set<FilePageId> spanningIds = regionsToPageIds(fileRegions);
    set<FilePageId>::const_iterator idIter;
    set<FilePageId>::const_iterator idEnd = spanningIds.end();
    for (idIter = spanningIds.begin(); idIter != idEnd; ++idIter)
    {
        FilePage fp(pageBeginOffset(*idIter), pageSize_);
        spanningPages.insert(fp);
    }
    return spanningPages;
}

FileDescriptor* PagedCache::getPageViewDescriptor(
    const Filename& filename, const set<size_t>& pageIds) const
{
    assert(!pageIds.empty());

    // Create the vector of displacements
    set<FilePageId>::const_iterator idIter = pageIds.begin();
    vector<size_t> displacements(pageIds.size());
    for (size_t i = 0; i < displacements.size(); i++)
    {
        displacements[i] = (*idIter) * pageSize_;
        ++idIter;
    }

    // Create the block indexed data type to use as the view
    BlockIndexedDataType* pageView = new BlockIndexedDataType(pageSize_,
                                                              displacements,
                                                              byteDataType_);
    FileView cacheView(0, pageView);

    // Create a descriptor with which to apply the view
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(filename);
    fd->setFileView(cacheView);
    return fd;
}

bool operator<(const PagedCache::Key& lhs, const PagedCache::Key& rhs)
{
    if (lhs.filename == rhs.filename)
    {
        return (lhs.key < rhs.key);
    }
    else
    {
        return (lhs.filename < rhs.filename);
    }
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
