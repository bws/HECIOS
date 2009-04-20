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
#include "file_page_utils.h"
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
    FilePageUtils& utils = FilePageUtils::instance();
    return utils.determineRequestPages(pageSize_, offset, size, view);
}

set<FilePageId> PagedCache::determineRequestFullPages(const FSOffset& offset,
                                                      const FSSize& size,
                                                      const FileView& view) const
{
    FilePageUtils& utils = FilePageUtils::instance();
    return utils.determineRequestFullPages(pageSize_, offset, size, view);
}

set<FilePageId> PagedCache::determineRequestPartialPages(const FSOffset& offset,
                                                         const FSSize& size,
                                                         const FileView& view) const
{
    FilePageUtils& utils = FilePageUtils::instance();
    return utils.determineRequestPartialPages(pageSize_, offset, size, view);
}

FileRegionSet PagedCache::determinePartialPageRegions(const FilePageId& pageId,
                                                      const FSOffset& offset,
                                                      const FSSize& size,
                                                      const FileView& view) const
{
    FilePageUtils& utils = FilePageUtils::instance();
    return utils.determinePartialPageRegions(pageSize_,
                                             pageId,
                                             offset,
                                             size,
                                             view);
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
    static DataType* byteType = new ByteDataType();
    spfsMPIFileReadAtRequest* readRequest =
        new spfsMPIFileReadAtRequest("PagedCache Read Request",
                                     SPFS_MPI_FILE_READ_AT_REQUEST);
    readRequest->setContextPointer(origRequest);
    readRequest->setFileDes(fd);
    readRequest->setDataType(byteType);
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
    static DataType* byteType = new ByteDataType();
    spfsMPIFileWriteAtRequest* writeRequest =
        new spfsMPIFileWriteAtRequest("PagedCache Write Request",
                                      SPFS_MPI_FILE_WRITE_AT_REQUEST);
    writeRequest->setContextPointer(origRequest);
    writeRequest->setFileDes(fd);
    writeRequest->setDataType(byteType);
    writeRequest->setCount(pageIds.size() * pageSize_);
    writeRequest->setOffset(0);
    return writeRequest;
}

FileDescriptor* PagedCache::getPageViewDescriptor(
    const Filename& filename, const set<size_t>& pageIds) const
{
    assert(!pageIds.empty());

    FilePageUtils& pageUtils = FilePageUtils::instance();
    FileView* cacheView = pageUtils.createPageViewDescriptor(pageSize_, pageIds);

    // Create a descriptor with which to apply the view
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(filename);
    fd->setFileView(*cacheView);
    delete cacheView;
    return fd;
}

bool operator<(const PagedCache::Key& lhs, const PagedCache::Key& rhs)
{
    if (lhs.key == rhs.key)
    {
        return (lhs.filename < rhs.filename);
    }
    else
    {
        return (lhs.key < rhs.key);
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
