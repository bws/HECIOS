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
#include "middleware_cache.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <omnetpp.h>
#include "basic_data_type.h"
#include "block_indexed_data_type.h"
#include "data_type_processor.h"
#include "file_builder.h"
#include "file_view.h"
#include "mpi_proto_m.h"
using namespace std;

MiddlewareCache::MiddlewareCache()
    : appInGateId_(-1),
      appOutGateId_(-1),
      fsInGateId_(-1),
      fsOutGateId_(-1),
      numCacheHits_(0),
      numCacheMisses_(0),
      numCacheEvicts_(0)
{
}

MiddlewareCache::~MiddlewareCache()
{
}

void MiddlewareCache::initialize()
{
    // Find gate ids
    appInGateId_ = findGate("appIn");
    appOutGateId_ = findGate("appOut");
    fsInGateId_ = findGate("fsIn");
    fsOutGateId_ = findGate("fsOut");

    // Initialize statistics
    numCacheHits_ = 0;
    numCacheMisses_ = 0;
    numCacheEvicts_ = 0;
}

void MiddlewareCache::finish()
{
    double totalCacheAccesses = numCacheHits_ + numCacheMisses_;
    double hitRate = numCacheHits_ / totalCacheAccesses;
    recordScalar("SPFS MWare Cache Hit Rate", hitRate);
    recordScalar("SPFS MWare Cache Evictions", numCacheEvicts_);
    recordScalar("SPFS MWare Cache Hits", numCacheHits_);
    recordScalar("SPFS MWare Cache Misses", numCacheMisses_);
}

void MiddlewareCache::handleMessage(cMessage* msg)
{
     if (msg->arrivalGateId() == appInGateId())
     {
         handleApplicationMessage(msg);
     }
     else if (msg->arrivalGateId() == fsInGateId())
     {
         handleFileSystemMessage(msg);
     }
     else
     {
         cerr << __FILE__ << ":" << __LINE__ << ":"
              <<  "message arrived through illegal gate: "
              << msg->info() << endl;
     }
}

//
// NoMiddlewareCache implementation
//
//
// OMNet Registriation Method
Define_Module(NoMiddlewareCache);

NoMiddlewareCache::NoMiddlewareCache()
{
}

// Perform simple pass through on all messages
void NoMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    send(msg, fsOutGateId());
}

void NoMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{
    send(msg, appOutGateId());
}


//
// PagedCache implementation
//
//
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
                                                  const FileView& view)
{
    // Flatten view into file regions for the correct size
    vector<FileRegion> requestRegions =
        DataTypeProcessor::locateFileRegions(offset, size, view);

    // Convert regions into file pages
    return regionsToPageIds(requestRegions);
}

set<FilePageId> PagedCache::determineRequestFullPages(const FSOffset& offset,
                                                      const FSSize& size,
                                                      const FileView& view)
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
                                                         const FileView& view)
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

void PagedCache::initialize()
{
    MiddlewareCache::initialize();
    pageSize_ = par("pageSize");
    pageCapacity_ = par("pageCapacity");
}

spfsMPIFileReadAtRequest* PagedCache::createPageReadRequest(
    const set<FilePageId>& pageIds,
    spfsMPIFileRequest* origRequest) const
{
    assert(!pageIds.empty());
    assert(0 != origRequest);

    // Construct a descriptor that views only the correct pages
    Filename name = origRequest->getFileDes()->getFilename();
    FileDescriptor* fd = getPageViewDescriptor(name, pageIds);

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
    const set<FilePageId>& pageIds,
    spfsMPIFileRequest* origRequest) const
{
    assert(!pageIds.empty());
    assert(0 != origRequest);

    // Construct a descriptor that views only the correct pages
    Filename name = origRequest->getFileDes()->getFilename();
    FileDescriptor* fd = getPageViewDescriptor(name, pageIds);

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

set<FilePageId> PagedCache::regionsToPageIds(const vector<FileRegion>& fileRegions)
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

set<FilePage> PagedCache::regionsToPages(const vector<FileRegion>& fileRegions)
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


//
// DirectPagedMiddlewareCache Implementation
//
//
// OMNet Registriation Method
Define_Module(DirectPagedMiddlewareCache);

DirectPagedMiddlewareCache::DirectPagedMiddlewareCache()
    : lruCache_(0)
{
}

void DirectPagedMiddlewareCache::initialize()
{
    // Initialize parent
    PagedCache::initialize();

    // Initialize LRU management
    assert(0 != cacheCapacity());
    lruCache_ = new LRUCache<std::size_t, FilePageId>(cacheCapacity());
}

void DirectPagedMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_READ_AT_REQUEST == msg->kind())
    {
        spfsMPIFileReadAtRequest* readAt =
            static_cast<spfsMPIFileReadAtRequest*>(msg);

        // Determine the pages remaining for this request
        set<FilePageId> remainingPages = resolveRequest(readAt);

        // Request unresolved pages
        if (!remainingPages.empty())
        {
            spfsMPIFileReadAtRequest* pageRead = createPageReadRequest(remainingPages,
                                                                   readAt);
            send(pageRead, fsOutGateId());
        }
    }
    else if (SPFS_MPI_FILE_WRITE_AT_REQUEST == msg->kind())
    {
        spfsMPIFileWriteAtRequest* writeAt =
            static_cast<spfsMPIFileWriteAtRequest*>(msg);

        // Retrieve any partial pages written by this request
        set<FilePageId> requestPages = resolveRequest(writeAt);

        // Update the cache with the full pages
        updateCache(writeAt);

        // If partial pages exist, read those pages
        if (!requestPages.empty())
        {
            spfsMPIFileReadAtRequest* pageRead = createPageReadRequest(requestPages,
                                                                       writeAt);
            send(pageRead, fsOutGateId());
        }
    }
    else
    {
        assert(SPFS_MPI_FILE_READ_REQUEST != msg->kind());
        assert(SPFS_MPI_FILE_WRITE_REQUEST != msg->kind());

        // Forward messages not handled by the cache
        send(msg, fsOutGateId());
    }

    // Complete any requests satisfied by this request
    completeRequests();
}

void DirectPagedMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{
    // Determine if the message is a write back
    if (0 == msg->contextPointer())
    {
        assert(SPFS_MPI_FILE_WRITE_AT_RESPONSE == msg->kind());
        cerr << __FILE__ << ":" << __LINE__ << ": "
             << "Cache Write-Back Complete" << endl;
    }
    else
    {
        if (SPFS_MPI_FILE_READ_AT_RESPONSE == msg->kind())
        {
            // Update the cache and any pending requests
            spfsMPIFileReadAtResponse* cacheRead =
                dynamic_cast<spfsMPIFileReadAtResponse*>(msg);
            assert(0 != cacheRead);
            updateCache(cacheRead);
        }
        else if (SPFS_MPI_FILE_WRITE_AT_RESPONSE == msg->kind())
        {
            // Do nothing
        }
        else
        {
            assert(SPFS_MPI_FILE_READ_RESPONSE != msg->kind());
            assert(SPFS_MPI_FILE_WRITE_RESPONSE != msg->kind());

            // Forward messages not handled by the cache
            send(msg, appOutGateId());
        }
    }

    // Complete any requests satisfied by this response
    completeRequests();
}

set<FilePageId> DirectPagedMiddlewareCache::resolveRequest(
    spfsMPIFileReadAtRequest* readAt)
{
    // Convert regions into file pages
    FileDescriptor* fd = readAt->getFileDes();
    FSSize readSize = readAt->getDataType()->getExtent() * readAt->getCount();
    set<FilePageId> requestPages = determineRequestPages(readAt->getOffset(),
                                                         readSize,
                                                         fd->getFileView());

    // Cull pages already in the cache
    set<FilePageId>::iterator iter;
    for (iter = requestPages.begin(); requestPages.end() != iter; /* nothing */)
    {
        cerr << "Checking cache for page " << *iter;
        if (lruCache_->exists(*iter))
        {
            // The call to erase invalidates the iterator, thus we have to
            // use post-increment in the erase call.  See item 9 in
            // Effective STL.
            requestPages.erase(iter++);
            cerr << " . . .  found" << endl;
        }
        else
        {
            ++iter;
            cerr << " . . .  NOT found" << endl;
        }
    }

    // Remove pages already requested (must be done before registering this
    // request)
    set<FilePageId> trimmedPages = removeRequestedPages(requestPages);

    // Register the request's pending pages
    registerPendingRequest(readAt, requestPages);

    return trimmedPages;
}

set<FilePageId> DirectPagedMiddlewareCache::resolveRequest(
    spfsMPIFileWriteAtRequest* writeAt)
{
    // Convert regions into file pages
    FileDescriptor* fd = writeAt->getFileDes();
    FSSize writeSize = writeAt->getDataType()->getExtent() * writeAt->getCount();
    set<FilePageId> partialPages =
        determineRequestPartialPages(writeAt->getOffset(),
                                     writeSize,
                                     fd->getFileView());

    // Remove pages already requested
    set<FilePageId> trimmedPages = removeRequestedPages(partialPages);

    // Register the request's pending pages
    registerPendingRequest(writeAt, partialPages);
    return trimmedPages;
}

void DirectPagedMiddlewareCache::updateCache(
    spfsMPIFileReadAtResponse* readAtResponse)
{
    // Extract the originating request
    cMessage* msg = static_cast<cMessage*>(readAtResponse->contextPointer());
    spfsMPIFileReadAtRequest* readAt = dynamic_cast<spfsMPIFileReadAtRequest*>(msg);
    assert(0 != readAt);

    // Convert regions into file pages
    FileDescriptor* fd = readAt->getFileDes();
    FSSize readSize = readAt->getDataType()->getExtent() * readAt->getCount();
    set<FilePageId> requestPages = determineRequestPages(readAt->getOffset(),
                                                         readSize,
                                                         fd->getFileView());

    // Update the cache and pending requests
    set<FilePageId> writeBacks;
    updateCache(requestPages, false, writeBacks);

    // Construct a request for the write-back pages
    if (!writeBacks.empty())
    {
        spfsMPIFileWriteAtRequest* writeback =
            createPageWriteRequest(writeBacks, readAt);
        send(writeback, fsOutGateId());
    }
}

void DirectPagedMiddlewareCache::updateCache(
    spfsMPIFileWriteAtRequest* writeAt)
{
    // Convert regions into file pages
    FileDescriptor* fd = writeAt->getFileDes();
    FSSize writeSize = writeAt->getDataType()->getExtent() * writeAt->getCount();
    set<FilePageId> fullPages =
        determineRequestFullPages(writeAt->getOffset(),
                                  writeSize,
                                  fd->getFileView());

    // Update the cache and pending requests
    set<FilePageId> writeBacks;
    updateCache(fullPages, true, writeBacks);

    // Construct a request for the write-back pages
    if (!writeBacks.empty())
    {
        spfsMPIFileWriteAtRequest* writeback =
            createPageWriteRequest(writeBacks, writeAt);
        send(writeback, fsOutGateId());
    }
}

void DirectPagedMiddlewareCache::updateCache(const set<FilePageId>& updatePages,
                                             bool updatesDirty,
                                             set<FilePageId>& outWriteBacks)
{
    // Update the cache and accumulate any writebacks required
    set<FilePageId>::const_iterator iter;
    for (iter = updatePages.begin(); iter != updatePages.end(); iter++)
    {
        try {
            FilePageId evictedPageId = 0;
            bool isDirty = false;
            // If the updates are dirty, then insert them into the cache
            // otherwise, if an existing entry isn't already dirty then
            //   insert the entry into the cache
            if (updatesDirty)
            {
                // Insert dirty entries
                lruCache_->insertAndRecall(*iter, *iter, true,
                                           evictedPageId, isDirty);
            }
            else if (!lruCache_->exists(*iter) ||
                     false == lruCache_->getDirtyBit(*iter))
            {
                // Insert clean entries that don't conflict with an existing
                // dirty entry
                lruCache_->insertAndRecall(*iter, *iter, false,
                                           evictedPageId, isDirty);
            }

            if (isDirty)
            {
                outWriteBacks.insert(evictedPageId);
            }
        } catch (const NoSuchEntry& e) {}
    }

    // Update pending requests with these pages
    updatePendingRequests(updatePages);
}

void DirectPagedMiddlewareCache::completeRequests()
{
    vector<spfsMPIFileRequest*> completedRequests = popCompletedRequests();

    for (size_t i = 0; i < completedRequests.size(); i++)
    {
        spfsMPIResponse* resp = 0;
        spfsMPIFileRequest* req = completedRequests[i];
        if (SPFS_MPI_FILE_READ_AT_REQUEST == req->kind())
        {
            resp =
                new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
            resp->setContextPointer(req);
        }
        else
        {
            assert(SPFS_MPI_FILE_WRITE_AT_REQUEST == req->kind());
            resp =
                new spfsMPIFileWriteAtResponse(0, SPFS_MPI_FILE_WRITE_AT_RESPONSE);
            resp->setContextPointer(req);
        }
        assert(0 != resp);
        send(resp, appOutGateId());
    }

    cerr << "Completed Reqs: " << completedRequests.size() << " Still pending: " << pendingRequests_.size() << endl;
}


void DirectPagedMiddlewareCache::registerPendingRequest(
    spfsMPIFileRequest* fileRequest, const set<FilePageId>& pendingPages)
{
    pendingRequests_[fileRequest] = pendingPages;
}

void DirectPagedMiddlewareCache::updatePendingRequests(const set<FilePageId>& pageIds)
{
    RequestMap::iterator iter;
    for (iter = pendingRequests_.begin(); pendingRequests_.end() != iter; ++iter)
    {
        set<FilePageId> result;
        set_difference(iter->second.begin(), iter->second.end(),
                       pageIds.begin(), pageIds.end(),
                       inserter(result, result.begin()));

        // Assign the resulting set as the new pending requests
        iter->second = result;
    }
}

vector<spfsMPIFileRequest*> DirectPagedMiddlewareCache::popCompletedRequests()
{
    vector<spfsMPIFileRequest*> completeRequests;
    RequestMap::iterator iter;
    for (iter = pendingRequests_.begin(); pendingRequests_.end() != iter; /* nothing */)
    {
        if (iter->second.empty())
        {
            completeRequests.push_back(iter->first);
            pendingRequests_.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
    return completeRequests;
}

set<FilePageId> DirectPagedMiddlewareCache::removeRequestedPages(set<FilePageId>& pageIds) const
{
    if (!pendingRequests_.empty())
    {
        set<FilePageId> result;
        RequestMap::const_iterator iter;
        for (iter = pendingRequests_.begin(); pendingRequests_.end() != iter; ++iter)
        {
            set_difference(pageIds.begin(), pageIds.end(),
                           iter->second.begin(), iter->second.end(),
                           inserter(result, result.begin()));
        }
        // Return the resulting set as the new set of trimmed pages
        return result;
    }
    return pageIds;
}


//
// CooperativeDirectMiddlewareCache Implementation
//
//
// OMNet Registriation Method
Define_Module(CooperativeDirectMiddlewareCache);

CooperativeDirectMiddlewareCache::CooperativeDirectMiddlewareCache()
{
}

void CooperativeDirectMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    assert(false);
}

void CooperativeDirectMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{
    assert(false);
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
