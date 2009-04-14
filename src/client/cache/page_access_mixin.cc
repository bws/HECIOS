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
#include "page_access_mixin.h"
#include <cassert>
#include "basic_data_type.h"
#include "block_indexed_data_type.h"
#include "cache_proto_m.h"
#include "file_builder.h"
#include "file_descriptor.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
using namespace std;

//
// Implementation for Page Access Mixin
//
PageAccessMixin::PageAccessMixin()
    : pageSize_(0)
{
}

PageAccessMixin::~PageAccessMixin()
{
}

FSSize PageAccessMixin::getPageSize() const
{
    return pageSize_;
}

void PageAccessMixin::setPageSize(const FSSize& pageSize)
{
    pageSize_ = pageSize;
}

size_t PageAccessMixin::getByteLength(const FileView& view)
{
    // Set the request size
    // (op, creds, fs_id, handle, dist, dist param, view, offset, data size)
    size_t length = 4 + FSClient::CREDENTIALS_SIZE + 4 + 8 + 4 + 4 + 4 +
                    view.getRepresentationByteLength() + 8 + 8;
    return length;
}

//
// Implementation for SinglePageAccessMixin
//
SinglePageAccessMixin::SinglePageAccessMixin()
{
}

SinglePageAccessMixin::~SinglePageAccessMixin()
{
}

vector<spfsCacheReadExclusiveRequest*>
SinglePageAccessMixin::createCacheReadExclusiveRequests(
    const std::set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    vector<spfsCacheReadExclusiveRequest*> requests;
    set<PagedCache::Key>::iterator iter = pages.begin();
    set<PagedCache::Key>::iterator end = pages.end();
    while (iter != end)
    {
        // Create the file descriptor
        Filename name = iter->filename;
        //FilePageId page = iter->key;
        //static DataType* byteType = new ByteDataType();
        FileDescriptor* fd = FileBuilder::instance().getDescriptor(name);
        spfsCacheReadExclusiveRequest* readPage =
            new spfsCacheReadExclusiveRequest("Single Page Cache Read Excl",
                                              SPFS_CACHE_READ_EXCLUSIVE_REQUEST);
        readPage->setContextPointer(parentRequest);
        readPage->setDescriptor(fd);
        //readPage->setDataType(byteType);
        //readPage->setOffset(page * getPageSize());
        //readPage->setCount(getPageSize());
        readPage->setByteLength(getByteLength(fd->getFileView()));

        // Add request and continue iterating
        requests.push_back(readPage);
        ++iter;

    }
    return requests;
}

vector<spfsCacheReadSharedRequest*>
SinglePageAccessMixin::createCacheReadSharedRequests(
    const std::set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    vector<spfsCacheReadSharedRequest*> requests;
    set<PagedCache::Key>::iterator iter = pages.begin();
    set<PagedCache::Key>::iterator end = pages.end();
    while (iter != end)
    {
        // Create the file descriptor
        Filename name = iter->filename;
        //FilePageId page = iter->key;
        //static DataType* byteType = new ByteDataType();
        FileDescriptor* fd = FileBuilder::instance().getDescriptor(name);
        spfsCacheReadSharedRequest* readPage =
            new spfsCacheReadSharedRequest("Single Page Cache Read Shared",
                                           SPFS_CACHE_READ_SHARED_REQUEST);
        readPage->setContextPointer(parentRequest);
        readPage->setDescriptor(fd);
        //readPage->setDataType(byteType);
        //readPage->setOffset(page * getPageSize());
        //readPage->setCount(getPageSize());
        readPage->setByteLength(getByteLength(fd->getFileView()));

        // Add request and continue iterating
        requests.push_back(readPage);
        ++iter;

    }
    return requests;
}

vector<spfsMPIFileReadAtRequest*>
SinglePageAccessMixin::createPFSReadRequests(
    const std::set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    vector<spfsMPIFileReadAtRequest*> requests;
    set<PagedCache::Key>::iterator iter = pages.begin();
    set<PagedCache::Key>::iterator end = pages.end();
    while (iter != end)
    {
        // Create the file descriptor
        Filename name = iter->filename;
        FilePageId page = iter->key;
        static DataType* byteType = new ByteDataType();
        FileDescriptor* fd = FileBuilder::instance().getDescriptor(name);
        spfsMPIFileReadAtRequest* readPage =
            new spfsMPIFileReadAtRequest("Single Page Read",
                                         SPFS_MPI_FILE_READ_AT_REQUEST);
        readPage->setContextPointer(parentRequest);
        readPage->setFileDes(fd);
        readPage->setDataType(byteType);
        readPage->setOffset(page * getPageSize());
        readPage->setCount(getPageSize());
        readPage->setByteLength(getByteLength(fd->getFileView()));

        // Add request and continue iterating
        requests.push_back(readPage);
        ++iter;

    }
    return requests;
}

vector<spfsMPIFileWriteAtRequest*>
SinglePageAccessMixin::createPFSWriteRequests(
    const std::set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    vector<spfsMPIFileWriteAtRequest*> requests;
    set<PagedCache::Key>::iterator iter = pages.begin();
    set<PagedCache::Key>::iterator end = pages.end();
    while (iter != end)
    {
        // Create the file descriptor
        Filename name = iter->filename;
        FilePageId page = iter->key;
        static DataType* byteType = new ByteDataType();
        FileDescriptor* fd = FileBuilder::instance().getDescriptor(name);
        spfsMPIFileWriteAtRequest* writePage =
            new spfsMPIFileWriteAtRequest("Single Page Write",
                                         SPFS_MPI_FILE_WRITE_AT_REQUEST);
        writePage->setContextPointer(parentRequest);
        writePage->setFileDes(fd);
        writePage->setDataType(byteType);
        writePage->setOffset(page * getPageSize());
        writePage->setCount(getPageSize());
        writePage->setByteLength(getByteLength(fd->getFileView()));

        // Add request and continue iterating
        requests.push_back(writePage);
        ++iter;

    }
    return requests;
}


//
// Implementation for BlockIndexedAccessMixin
//
BlockIndexedPageAccessMixin::BlockIndexedPageAccessMixin()
{
}

BlockIndexedPageAccessMixin::~BlockIndexedPageAccessMixin()
{
}

void BlockIndexedPageAccessMixin::groupPagesByFilename(
    const set<PagedCache::Key>& pageKeys,
    FilePageMap& outPageGroups) const
{
    assert(0 == outPageGroups.size());
    set<PagedCache::Key>::const_iterator iter = pageKeys.begin();
    set<PagedCache::Key>::const_iterator last = pageKeys.end();
    while (iter != last)
    {
        set<FilePageId>& pages = outPageGroups[iter->filename];
        pages.insert(iter->key);
    }
}
vector<spfsCacheReadExclusiveRequest*>
BlockIndexedPageAccessMixin::createCacheReadExclusiveRequests(
    const set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    // Group the pages
    FilePageMap pagesByName;
    groupPagesByFilename(pages, pagesByName);

    // Create the requests for each file
    vector<spfsCacheReadExclusiveRequest*> requests;
    FilePageMap::iterator iter = pagesByName.begin();
    FilePageMap::iterator last = pagesByName.end();
    while (iter != last)
    {
        // Construct a descriptor that views only the correct pages
        FileDescriptor* fd = getPageViewDescriptor(iter->first, iter->second);

        // Create the request
        //static DataType* byteType = new ByteDataType();
        spfsCacheReadExclusiveRequest* readRequest =
            new spfsCacheReadExclusiveRequest("Cache Read Excl Request",
                                              SPFS_CACHE_READ_EXCLUSIVE_REQUEST);
        readRequest->setContextPointer(parentRequest);
        readRequest->setDescriptor(fd);
        //readRequest->setDataType(byteType);
        //readRequest->setCount(pages.size() * getPageSize());
        //readRequest->setOffset(0);
        //readRequest->setByteLength(getByteLength(fd->getFileView()));

        // Add the request and increment iterator
        requests.push_back(readRequest);
        ++iter;
    }
    return requests;
}

vector<spfsCacheReadSharedRequest*>
BlockIndexedPageAccessMixin::createCacheReadSharedRequests(
    const set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    // Group the pages
    FilePageMap pagesByName;
    groupPagesByFilename(pages, pagesByName);

    // Create the requests for each file
    vector<spfsCacheReadSharedRequest*> requests;
    FilePageMap::iterator iter = pagesByName.begin();
    FilePageMap::iterator last = pagesByName.end();
    while (iter != last)
    {
        // Construct a descriptor that views only the correct pages
        FileDescriptor* fd = getPageViewDescriptor(iter->first, iter->second);

        // Create the request
        //static DataType* byteType = new ByteDataType();
        spfsCacheReadSharedRequest* readRequest =
            new spfsCacheReadSharedRequest("Cache Read Shared Request",
                                           SPFS_CACHE_READ_SHARED_REQUEST);
        readRequest->setContextPointer(parentRequest);
        readRequest->setDescriptor(fd);
        //readRequest->setDataType(byteType);
        //readRequest->setCount(pages.size() * getPageSize());
        //readRequest->setOffset(0);
        readRequest->setByteLength(getByteLength(fd->getFileView()));

        // Add the request and increment iterator
        requests.push_back(readRequest);
        ++iter;
    }
    return requests;
}

vector<spfsMPIFileReadAtRequest*>
BlockIndexedPageAccessMixin::createPFSReadRequests(
    const set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    // Group the pages
    FilePageMap pagesByName;
    groupPagesByFilename(pages, pagesByName);

    // Create the requests for each file
    vector<spfsMPIFileReadAtRequest*> requests;
    FilePageMap::iterator iter = pagesByName.begin();
    FilePageMap::iterator last = pagesByName.end();
    while (iter != last)
    {
        // Construct a descriptor that views only the correct pages
        FileDescriptor* fd = getPageViewDescriptor(iter->first, iter->second);

        // Create the request
        static DataType* byteType = new ByteDataType();
        spfsMPIFileReadAtRequest* readRequest =
            new spfsMPIFileReadAtRequest("PagedCache Read Request",
                                          SPFS_MPI_FILE_READ_AT_REQUEST);
        readRequest->setContextPointer(parentRequest);
        readRequest->setFileDes(fd);
        readRequest->setDataType(byteType);
        readRequest->setCount(pages.size() * getPageSize());
        readRequest->setOffset(0);
        readRequest->setByteLength(getByteLength(fd->getFileView()));

        // Add the request and increment iterator
        requests.push_back(readRequest);
        ++iter;
    }
    return requests;
}

/** @return a request to write the desired pages */
vector<spfsMPIFileWriteAtRequest*>
BlockIndexedPageAccessMixin::createPFSWriteRequests(
    const set<PagedCache::Key>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    // Group the pages
    FilePageMap pagesByName;
    groupPagesByFilename(pages, pagesByName);

    // Create the requests for each file
    vector<spfsMPIFileWriteAtRequest*> requests;
    FilePageMap::iterator iter = pagesByName.begin();
    FilePageMap::iterator last = pagesByName.end();
    while (iter != last)
    {
        // Construct a descriptor that views only the correct pages
        FileDescriptor* fd = getPageViewDescriptor(iter->first, iter->second);

        // Create the write request
        static DataType* byteType = new ByteDataType();
        spfsMPIFileWriteAtRequest* writeRequest =
            new spfsMPIFileWriteAtRequest("PagedCache Write Request",
                                          SPFS_MPI_FILE_WRITE_AT_REQUEST);
        writeRequest->setContextPointer(parentRequest);
        writeRequest->setFileDes(fd);
        writeRequest->setDataType(byteType);
        writeRequest->setCount(pages.size() * getPageSize());
        writeRequest->setOffset(0);
        writeRequest->setByteLength(getByteLength(fd->getFileView()));

        // Add the request and increment iterator
        requests.push_back(writeRequest);
        ++iter;
    }
    return requests;
}

FileDescriptor* BlockIndexedPageAccessMixin::getPageViewDescriptor(
    const Filename& filename, const set<size_t>& pageIds) const
{
    assert(!pageIds.empty());
    FSSize pageSize = getPageSize();

    // Create the vector of displacements
    set<FilePageId>::const_iterator idIter = pageIds.begin();
    vector<size_t> displacements(pageIds.size());
    for (size_t i = 0; i < displacements.size(); i++)
    {
        displacements[i] = (*idIter) * pageSize;
        ++idIter;
    }

    // Create the block indexed data type to use as the view
    BlockIndexedDataType* pageView = new BlockIndexedDataType(pageSize,
                                                              displacements,
                                                              byteDataType_);
    FileView cacheView(0, pageView);

    // Create a descriptor with which to apply the view
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(filename);
    fd->setFileView(cacheView);
    return fd;
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
