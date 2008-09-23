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
        // Only need to check if the first page begins on a boundary
        // and if the last page ends on a boundary.  Double insertion is
        // not a problems since we are using the stl::set datatype
        FSOffset begin = requestRegions[i].offset;
        if (0 != (begin % pageSize_))
        {
            size_t pageId = begin / pageSize_;
            partialPageIds.insert(pageId);
        }

        FSOffset end = begin + requestRegions[i].extent;
        if (pageSize_ != (end % pageSize_))
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
    spfsMPIFileReadRequest* origRequest) const
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
    spfsMPIFileWriteRequest* origRequest) const
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
        set<FilePageId> remainingPages = lookupData(readAt);

        // Request the pages from the file system (but not pages
        // that are already in transit)
        trimRequestedPages(remainingPages);
        registerPendingRequest(readAt, remainingPages);
        if (!remainingPages.empty())
        {
            spfsMPIFileReadAtRequest* pageRead = createPageReadRequest(remainingPages,
                                                                   readAt);
            send(pageRead, fsOutGateId());
        }
    }
    else if (SPFS_MPI_FILE_WRITE_AT_REQUEST == msg->kind())
    {

    }
    else
    {
        assert(SPFS_MPI_FILE_READ_REQUEST != msg->kind());
        assert(SPFS_MPI_FILE_WRITE_REQUEST != msg->kind());

        // Forward messages not handled by the cache
        send(msg, fsOutGateId());
    }

    // Complete any requests satisfied by this request
    popCompletedRequests();
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
            populateData(cacheRead);

            // Complete requests satisfied by this read
            vector<spfsMPIFileReadAtRequest*> completeReqs = popCompletedRequests();
            for (size_t i = 0; i < completeReqs.size(); i++)
            {
                spfsMPIFileReadAtResponse* readAtResp =
                    new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
                readAtResp->setContextPointer(completeReqs[i]);
                send(readAtResp, appOutGateId());
            }
        }
        else
        {
            assert(SPFS_MPI_FILE_READ_RESPONSE != msg->kind());
            assert(SPFS_MPI_FILE_WRITE_RESPONSE != msg->kind());

            // Forward messages not handled by the cache
            send(msg, appOutGateId());
        }
    }
}

set<FilePageId> DirectPagedMiddlewareCache::lookupData(
    spfsMPIFileReadAtRequest* readAt)
{
    // Convert regions into file pages
    FileDescriptor* fd = readAt->getFileDes();
    FSSize readSize = readAt->getDataType()->getExtent() * readAt->getCount();
    set<FilePageId> requestPages = determineRequestPages(readAt->getOffset(),
                                                         readSize,
                                                         fd->getFileView());

    // Remove pages already in the cache
    set<FilePageId>::iterator iter;
    for (iter = requestPages.begin(); requestPages.end() != iter; /* nothing */)
    {
        if (lruCache_->exists(*iter))
        {
            // The call to erase invalidates the iterator, thus we have to
            // use post-increment in the erase call.  See item 9 in
            // Effective STL.
            requestPages.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
    return requestPages;
}

void DirectPagedMiddlewareCache::populateData(
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

    // Update pages in the cache that aren't marked dirty
    set<FilePageId>::const_iterator iter;
    const set<FilePageId>::const_iterator iterEnd = requestPages.end();
    for (iter = requestPages.begin(); iterEnd != iter; ++iter)
    {
        try
        {
            if (!lruCache_->getDirtyBit(*iter))
            {
                lruCache_->insert(*iter, *iter);
            }
        } catch(NoSuchEntry& exc)
        {
            lruCache_->insert(*iter, *iter);
        }
    }

    // Update pending requests with these pages
    updatePendingRequests(requestPages);
}

void DirectPagedMiddlewareCache::registerPendingRequest(
    spfsMPIFileReadAtRequest* fileRequest, const set<FilePageId>& pendingPages)
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

vector<spfsMPIFileReadAtRequest*> DirectPagedMiddlewareCache::popCompletedRequests()
{
    vector<spfsMPIFileReadAtRequest*> completeRequests;
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

void DirectPagedMiddlewareCache::trimRequestedPages(set<FilePageId>& pageIds) const
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
        // Assign the resulting set as the new set of trimmed pages
        pageIds = result;
    }
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
