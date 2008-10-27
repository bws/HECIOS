//
// This file is part of Hecios
//
// Copyright (C) 2008 bradles
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
#include "direct_paged_middleware_cache.h"
#include <algorithm>
#include <cassert>
#include "mpi_proto_m.h"
using namespace std;

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
    lruCache_ = createFileDataPageCache(cacheCapacity());

    // Initialize the pending request map
    pendingRequests_ = createPendingRequestMap();

    // Initialize the open file map
    openFileCounts_ = createOpenFileMap();
}

DirectPagedMiddlewareCache::FileDataPageCache*
DirectPagedMiddlewareCache::createFileDataPageCache(size_t cacheSize)
{
    return new FileDataPageCache(cacheSize);
}

DirectPagedMiddlewareCache::RequestMap*
DirectPagedMiddlewareCache::createPendingRequestMap()
{
    return new RequestMap();
}

DirectPagedMiddlewareCache::OpenFileMap*
DirectPagedMiddlewareCache::createOpenFileMap()
{
    return new OpenFileMap();
}

void DirectPagedMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_OPEN_REQUEST == msg->kind())
    {
        spfsMPIFileOpenRequest* open = static_cast<spfsMPIFileOpenRequest*>(msg);
        Filename openFile(open->getFileName());
        processFileOpen(openFile);
        send(msg, fsOutGateId());
    }
    else if (SPFS_MPI_FILE_CLOSE_REQUEST == msg->kind())
    {
        spfsMPIFileCloseRequest* close = static_cast<spfsMPIFileCloseRequest*>(msg);
        FileDescriptor* fd = close->getFileDes();
        processFileClose(fd->getFilename());
        send(msg, fsOutGateId());
    }
    else if (SPFS_MPI_FILE_READ_AT_REQUEST == msg->kind())
    {
        spfsMPIFileReadAtRequest* readAt =
            static_cast<spfsMPIFileReadAtRequest*>(msg);

        // Determine the pages remaining for this request
        set<FilePageId> remainingPages = resolveRequest(readAt);

        // Request unresolved pages
        if (!remainingPages.empty())
        {
            spfsMPIFileReadAtRequest* pageRead =
                createPageReadRequest(readAt->getFileDes()->getFilename(),
                                      remainingPages,
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
            spfsMPIFileReadAtRequest* pageRead =
                createPageReadRequest(writeAt->getFileDes()->getFilename(),
                                      requestPages,
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

            // Clean up the request-response pair
            cMessage* request = static_cast<cMessage*>(msg->contextPointer());
            delete request;
            delete msg;
        }
        else if (SPFS_MPI_FILE_WRITE_AT_RESPONSE == msg->kind())
        {
            // Clean up the request-response pair
            cMessage* request = static_cast<cMessage*>(msg->contextPointer());
            delete request;
            delete msg;
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

void DirectPagedMiddlewareCache::processFileOpen(const Filename& openName)
{
    if (0 == openFileCounts_->count(openName))
    {
        (*openFileCounts_)[openName] = 1;
    }
    else
    {
        (*openFileCounts_)[openName]++;
    }
}

void DirectPagedMiddlewareCache::processFileClose(const Filename& closeName)
{
    assert(0 != openFileCounts_->count(closeName));

    // Decrement the count
    size_t activeHandles = --((*openFileCounts_)[closeName]);

    // Flush cache data if this is the final close
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "After close, num active handles is: " << activeHandles << endl;
    if (0 == activeHandles)
    {
        // TODO: Flush the data here somehow
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "TODO: Need to flush file data on close!!" << endl;
    }
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
        PagedCache::Key k(fd->getFilename(), *iter);
        if (lruCache_->exists(k))
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
    set<PagedCache::Key> writeBacks;
    updateCache(fd->getFilename(), requestPages, false, writeBacks);

    // Construct a request for the write-back pages
    completeWriteBacks(writeBacks, readAt);
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
    set<PagedCache::Key> writeBacks;
    updateCache(fd->getFilename(), fullPages, true, writeBacks);

    // Construct a request for the write-back pages
    completeWriteBacks(writeBacks, writeAt);
}

void DirectPagedMiddlewareCache::updateCache(const Filename& filename,
                                             const set<FilePageId>& updatePages,
                                             bool updatesDirty,
                                             set<PagedCache::Key>& outWriteBacks)
{
    // Update the cache and accumulate any writebacks required
    set<FilePageId>::const_iterator iter;
    for (iter = updatePages.begin(); iter != updatePages.end(); iter++)
    {
        try {
            PagedCache::Key evictedKey(Filename("/"), 0);
            FilePageId evictedValue = 0;
            bool isDirty = false;
            // If the updates are dirty, then insert them into the cache
            // otherwise, if an existing entry isn't already dirty then
            //   insert the entry into the cache
            PagedCache::Key insertKey(filename, *iter);
            if (updatesDirty)
            {
                // Insert dirty entries
                lruCache_->insertAndRecall(insertKey, *iter, true,
                                           evictedKey, evictedValue, isDirty);
            }
            else if (!lruCache_->exists(insertKey) ||
                     false == lruCache_->getDirtyBit(insertKey))
            {
                // Insert clean entries that don't conflict with an existing
                // dirty entry
                lruCache_->insertAndRecall(insertKey, *iter, false,
                                           evictedKey, evictedValue, isDirty);
            }

            if (isDirty)
            {
                outWriteBacks.insert(evictedKey);
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
        else if (SPFS_MPI_FILE_WRITE_AT_REQUEST == req->kind())
        {
            resp =
                new spfsMPIFileWriteAtResponse(0, SPFS_MPI_FILE_WRITE_AT_RESPONSE);
            resp->setContextPointer(req);
        }
        else
        {
            assert(SPFS_MPI_FILE_CLOSE_REQUEST == req->kind());
            resp =
                new spfsMPIFileCloseResponse(0, SPFS_MPI_FILE_CLOSE_RESPONSE);
            resp->setContextPointer(req);
        }
        assert(0 != resp);
        send(resp, appOutGateId());
    }
}

void DirectPagedMiddlewareCache::completeWriteBacks(const set<PagedCache::Key>& writeBacks,
                                                    spfsMPIFileRequest* req)
{
    if (!writeBacks.empty())
    {
        // Accumulate the pages for each file and issue a writeback
        set<FilePageId> pages;
        Filename currentName = writeBacks.begin()->filename;
        set<PagedCache::Key>::iterator iter;
        for (iter = writeBacks.begin(); iter != writeBacks.end(); ++iter)
        {
            if (currentName == iter->filename)
            {
                pages.insert(iter->key);
            }
            else
            {
                // Write back the accumulated pages
                spfsMPIFileWriteAtRequest* writeBackRequest =
                    createPageWriteRequest(currentName, pages, req);
                send(writeBackRequest, fsOutGateId());

                // Reset the loop state and append the page to the new set
                pages.clear();
                currentName = iter->filename;
                pages.insert(iter->key);
            }
        }

        // Write back the pages
        spfsMPIFileWriteAtRequest* writeBackRequest =
            createPageWriteRequest(currentName, pages, req);
        send(writeBackRequest, fsOutGateId());
    }
}

void DirectPagedMiddlewareCache::registerPendingRequest(
    spfsMPIFileRequest* fileRequest, const set<FilePageId>& pendingPages)
{
    (*pendingRequests_)[fileRequest] = pendingPages;
}

void DirectPagedMiddlewareCache::updatePendingRequests(const set<FilePageId>& pageIds)
{
    RequestMap::iterator iter;
    for (iter = pendingRequests_->begin(); pendingRequests_->end() != iter; ++iter)
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
    for (iter = pendingRequests_->begin(); pendingRequests_->end() != iter; /* nothing */)
    {
        if (iter->second.empty())
        {
            completeRequests.push_back(iter->first);
            pendingRequests_->erase(iter++);
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
    if (!pendingRequests_->empty())
    {
        set<FilePageId> result;
        RequestMap::const_iterator iter;
        for (iter = pendingRequests_->begin(); pendingRequests_->end() != iter; ++iter)
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


/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
