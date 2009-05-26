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
#include "progressive_paged_middleware_cache.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include "comm_man.h"
#include "dirty_file_region_set.h"
#include "file_page_utils.h"
#include "mpi_proto_m.h"
#include "page_access_mixin.h"
#include "paged_cache.h"
#include "progressive_page_access_strategy.h"
using namespace std;

// OMNet Registration Method
Define_Module(ProgressivePagedMiddlewareCache);

/** Functor for finding dirty pages for a cached file */
class DirtyPageFilter : public ProgressivePagedMiddlewareCache::FileDataPageCache::FilterFunctor
{
public:
    /** Constructor */
    DirtyPageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file and is dirty */
    virtual bool filter(const ProgressivePagedMiddlewareCache::Key& key,
                        const ProgressivePagedMiddlewareCache::ProgressivePage& entry,
                        bool isDirty) const
    {
        return ((key.filename == filename_) && (isDirty));
    };

private:
    Filename filename_;
};

/** Functor for finding all pages for a cached file */
class FilePageFilter : public ProgressivePagedMiddlewareCache::FileDataPageCache::FilterFunctor
{
public:
    /** Constructor */
    FilePageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file and is dirty */
    virtual bool filter(const ProgressivePagedMiddlewareCache::Key& key,
                        const ProgressivePagedMiddlewareCache::ProgressivePage& entry,
                        bool isDirty) const
    {
        return (key.filename == filename_);
    };

private:
    Filename filename_;
};


ProgressivePagedMiddlewareCache::ProgressivePagedMiddlewareCache()
    : lruCache_(0),
      openFileCounts_(0),
      pageCapacity_(0),
      pageSize_(0),
      pendingRequests_(0)
{
}

void ProgressivePagedMiddlewareCache::initialize()
{
    // Initialize parent
    MiddlewareCache::initialize();

    // Extract cache paramters
    pageCapacity_ = par("pageCapacity");
    pageSize_ = par("pageSize");

    // Initialize LRU management
    assert(0 != pageCapacity_);
    lruCache_ = createFileDataPageCache(pageCapacity_);

    // Initialize the pending request maps
    pendingRequests_ = createPendingDataMap();

    // Initialize the open file map
    openFileCounts_ = createOpenFileMap();

    // Initialize statistical data
    recordScalars_ = false;
    maxPageRegions_ = 0;
}

void ProgressivePagedMiddlewareCache::finish()
{
    recordScalar("SPFS Max Progressive Page Regions", maxPageRegions_);

    MiddlewareCache::finish();
}

ProgressivePagedMiddlewareCache::FileDataPageCache*
ProgressivePagedMiddlewareCache::createFileDataPageCache(size_t cacheSize)
{
    return new FileDataPageCache(cacheSize);
}

ProgressivePagedMiddlewareCache::RequestMap*
ProgressivePagedMiddlewareCache::createPendingDataMap()
{
    return new RequestMap();
}

ProgressivePagedMiddlewareCache::OpenFileMap*
ProgressivePagedMiddlewareCache::createOpenFileMap()
{
    return new OpenFileMap();
}

void ProgressivePagedMiddlewareCache::performFakeOpen(const Filename& openName)
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


void ProgressivePagedMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_OPEN_REQUEST == msg->kind() ||
        SPFS_MPI_FILE_CLOSE_REQUEST == msg->kind() ||
        SPFS_MPI_FILE_READ_AT_REQUEST == msg->kind() ||
        SPFS_MPI_FILE_WRITE_AT_REQUEST == msg->kind())
    {
        processRequest(msg, msg);
    }
    else
    {
        assert(SPFS_MPI_FILE_READ_REQUEST != msg->kind());
        assert(SPFS_MPI_FILE_WRITE_REQUEST != msg->kind());

        // Forward messages not handled by the cache
        send(msg, fsOutGateId());
    }
}

void ProgressivePagedMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_READ_AT_RESPONSE == msg->kind() ||
        SPFS_MPI_FILE_WRITE_AT_RESPONSE == msg->kind())
    {
        cMessage* cacheRequest = static_cast<cMessage*>(msg->contextPointer());
        cMessage* appRequest = static_cast<cMessage*>(cacheRequest->contextPointer());
        processRequest(appRequest, msg);

        // Don't delete bypass writes
        if (appRequest != cacheRequest)
        {
            delete cacheRequest;
            delete msg;
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

void ProgressivePagedMiddlewareCache::processRequest(cMessage* request, cMessage* msg)
{
    if (0 == request)
    {
        // Extract the writeback pages and resolve the associated pages
        assert(0 != dynamic_cast<spfsMPIFileWriteAtResponse*>(msg));
        spfsMPIFileWriteAtRequest* writeback =
            static_cast<spfsMPIFileWriteAtRequest*>(msg->contextPointer());

        // Determine the cache pages requested
        resolvePendingRequest(writeback);
    }
    else if (SPFS_MPI_FILE_OPEN_REQUEST == request->kind())
    {
        spfsMPIFileOpenRequest* open = static_cast<spfsMPIFileOpenRequest*>(request);
        processFileOpen(open, msg);
    }
    else if (SPFS_MPI_FILE_CLOSE_REQUEST == request->kind())
    {
        spfsMPIFileCloseRequest* close = static_cast<spfsMPIFileCloseRequest*>(request);
        processFileClose(close, msg);
    }
    else if (SPFS_MPI_FILE_READ_AT_REQUEST == request->kind())
    {
        spfsMPIFileReadAtRequest* readAt =
            static_cast<spfsMPIFileReadAtRequest*>(request);
        processFileRead(readAt, msg);
    }
    else if (SPFS_MPI_FILE_WRITE_AT_REQUEST == request->kind())
    {
        spfsMPIFileWriteAtRequest* writeAt =
            static_cast<spfsMPIFileWriteAtRequest*>(request);
        processFileWrite(writeAt, msg);
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "ERROR: Unknown request type: " << request->kind()
             << " Message: " << msg->info() << endl;
        assert(0);
    }

    // Complete any requests satisfied by this message
    completeRequests();
}

void ProgressivePagedMiddlewareCache::processFileOpen(spfsMPIFileOpenRequest* open,
                                                 cMessage* msg)
{
    if (msg == open)
    {
        // Increment the open file count
        Filename openName(open->getFileName());
        if (0 == openFileCounts_->count(openName))
        {
            (*openFileCounts_)[openName] = 1;
        }
        else
        {
            (*openFileCounts_)[openName]++;
        }

        // Send the open request on to the file system
        send(msg, fsOutGateId());
    }
    else
    {
        // Send the open response on to the file system
        send(msg, appOutGateId());
    }
}

void ProgressivePagedMiddlewareCache::processFileClose(spfsMPIFileCloseRequest* close, cMessage* msg)
{
    if (msg == close)
    {
        // Extract the filename
        Filename closeName = close->getFileDes()->getFilename();
        assert(0 != openFileCounts_->count(closeName));
        assert(0 != (*openFileCounts_)[closeName]);

        // Decrement the open count
        size_t openCount = --((*openFileCounts_)[closeName]);

        // If this is the last close, flush the dirty cache data to disk
        if (0 == openCount)
        {
            vector<WritebackPage> writePages = lookupDirtyPagesInCache(closeName);
            beginWritebackEvictions(writePages, close);
            flushCache(closeName);
        }
        else
        {
            // Send the close request on to the file system
            send(msg, fsOutGateId());
        }
    }
    else
    {
        assert(0 != dynamic_cast<spfsMPIFileWriteAtResponse*>(msg));
        spfsMPIFileWriteAtRequest* flushReq =
            static_cast<spfsMPIFileWriteAtRequest*>(msg->contextPointer());

        // Resolve the pending request
        resolvePendingRequest(flushReq);
    }
}

void ProgressivePagedMiddlewareCache::processFileRead(spfsMPIFileReadAtRequest* read, cMessage* msg)
{
    if (msg == read)
    {
        // Determine the cache pages requested
        set<Key> requestPages;
        getRequestCachePages(read, requestPages);

        // Remove cached pages
        lookupPagesInCache(requestPages);

        // Cull read pages that are already requested
        set<Key> trimmedPages = trimPendingReadPages(requestPages);

        // Begin the read
        beginRead(trimmedPages, read);

        // Register the request's pending pages
        registerPendingReadPages(read, requestPages);

        // Set the total memory delay
        addCacheMemoryDelay(read, 0.0);
    }
    else
    {
        assert(0 != dynamic_cast<spfsMPIFileReadAtResponse*>(msg));

        // Determine the set of read pages
        spfsMPIFileReadAtRequest* cacheRead =
            static_cast<spfsMPIFileReadAtRequest*>(msg->contextPointer());

        // Determine the cache pages read
        set<Key> requestPages;
        getRequestCachePages(cacheRead, requestPages);
        resolvePendingPages(requestPages);

        // Update the cache
        vector<WritebackPage> writebackPages;
        updateCache(requestPages, false, writebackPages);

        // The writebuffer is not infinite, the request will need
        // to pause while writebacks occur
        if (!writebackPages.empty())
        {
            beginWritebackEvictions(writebackPages, 0);
        }
    }
}

void ProgressivePagedMiddlewareCache::processFileWrite(spfsMPIFileWriteAtRequest* write, cMessage* msg)
{
    // Cache write state machine states
    enum {
        INIT = 0,
        BEGIN_CACHE_BYPASS_WRITE = FSM_Steady(1),
        COMPLETE_CACHE_BYPASS_WRITE = FSM_Steady(2),
        UPDATE_CACHE = FSM_Steady(4),
        BEGIN_WRITEBACK = FSM_Steady(5),
    };

    // Variables needed for multiple states
    set<PagedCache::Key> partialPagesTrimmed;

    cFSM currentState = write->getCacheState();
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            // Determine the cache pages requested
            set<Key> requestPages;
            getRequestCachePages(write, requestPages);

            if (requestPages.size() > lruCache_->capacity())
            {
                FSM_Goto(currentState, BEGIN_CACHE_BYPASS_WRITE);
            }
            else
            {
                // Set the total memory delay
                double delay = byteCopyTime() * write->getCount() *
                    write->getDataType()->getTrueExtent();
                addCacheMemoryDelay(write, delay);

                // Register the request with teh empty set so that it will
                // complete correctly if no writebacks are required
                set<Key> empty;
                registerPendingReadPages(write, empty);

                FSM_Goto(currentState, UPDATE_CACHE);
            }
            break;
        }
        case FSM_Enter(BEGIN_CACHE_BYPASS_WRITE):
        {
            assert(write == msg);
            // Set the message context to itself so that messages processes
            msg->setContextPointer(msg);
            send(msg, fsOutGateId());
            break;
        }
        case FSM_Exit(BEGIN_CACHE_BYPASS_WRITE):
        {
            FSM_Goto(currentState, COMPLETE_CACHE_BYPASS_WRITE);
            break;
        }
        case FSM_Enter(COMPLETE_CACHE_BYPASS_WRITE):
        {
            send(msg, appOutGateId());
            break;
        }
        case FSM_Exit(COMPLETE_CACHE_BYPASS_WRITE):
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Illegal state reached: " << msg->info() << endl;
            assert(0);
            break;
        }
        case FSM_Enter(UPDATE_CACHE):
        {
            // Update the cache and acquire any necessary writebacks
            vector<WritebackPage> writebackPages;
            updateCache(write, writebackPages);

            // The writeback buffer is not infinite, the request will need
            // to pause while writebacks occur
            if (!writebackPages.empty())
            {
                beginWritebackEvictions(writebackPages, 0);
            }
            break;
        }
        case FSM_Exit(UPDATE_CACHE):
        {
              break;
        }
    }

    // Set the cache fsm state back into the request
    write->setCacheState(currentState);
}

template<class spfsMPIFileIORequest>
void ProgressivePagedMiddlewareCache::getRequestCachePages(
    const spfsMPIFileIORequest* ioRequest,
    std::set<Key>& outRequestPages) const
{
    // Convert regions into file pages
    FilePageUtils& utils = FilePageUtils::instance();
    FileDescriptor* fd = ioRequest->getFileDes();
    FSOffset offset = ioRequest->getOffset();
    FSSize size = ioRequest->getDataType()->getExtent() * ioRequest->getCount();
    set<FilePageId> requestPages = utils.determineRequestPages(pageSize_,
                                                               offset,
                                                               size,
                                                               fd->getFileView());

    // Convert file pages into cache keys
    set<FilePageId>::const_iterator iter = requestPages.begin();
    set<FilePageId>::const_iterator end = requestPages.end();
    while (iter != end)
    {
        Key k(fd->getFilename(), *(iter++));
        outRequestPages.insert(k);
    }
}

void ProgressivePagedMiddlewareCache::beginRead(const set<Key>& readPages,
                                                spfsMPIFileRequest* parentRequest)
{
    assert(0 != readPages.size());

    // Convert into paged cache keys
    set<Key>::iterator iter = readPages.begin();
    set<Key>::iterator end = readPages.end();
    set<PagedCache::Key> pageKeys;
    while (iter != end)
    {
        PagedCache::Key key(iter->filename, iter->key);
        pageKeys.insert(key);
        ++iter;
    }

    // Construct and send the requests
    BlockIndexedPageAccessMixin pageAccessor(pageSize_);
    vector<spfsMPIFileReadAtRequest*> reads =
        pageAccessor.createPFSReadRequests(pageKeys, parentRequest);
    for (size_t i = 0; i < reads.size(); i++)
    {
        send(reads[i], fsOutGateId());
    }
}


void ProgressivePagedMiddlewareCache::beginWritebackEvictions(
    const vector<WritebackPage>& writebackPages,
    spfsMPIFileRequest* parentRequest)
{
    // Use the progresive page write strategy to construct write requests
    ProgressivePageAccessStrategy pageAccessor(pageSize_);
    vector<spfsMPIFileWriteAtRequest*> writes =
        pageAccessor.createPFSWriteRequests(writebackPages, parentRequest);

    for (size_t i = 0; i < writes.size(); i++)
    {
        if (recordScalars_)
        {
            // Determine the max number of regions
            maxPageRegions_ = 0;
        }
        send(writes[i], fsOutGateId());
    }
    registerPendingWriteRequests(parentRequest, writes);
}

vector<ProgressivePagedMiddlewareCache::WritebackPage>
ProgressivePagedMiddlewareCache::lookupDirtyPagesInCache(const Filename& filename) const
{
    DirtyPageFilter filter(filename);
    vector<ProgressivePage*> dirtyPages = lruCache_->getFilteredEntries(filter);
    vector<WritebackPage> writebacks;
    for (size_t i = 0; i < dirtyPages.size(); i++)
    {
        WritebackPage writeback = {filename, dirtyPages[i]->id, dirtyPages[i]->regions};
        writebacks.push_back(writeback);
    }
    return writebacks;
}

void ProgressivePagedMiddlewareCache::lookupPagesInCache(set<Key>& requestPages)
{
    set<Key>::iterator iter = requestPages.begin();
    set<Key>::iterator end = requestPages.end();
    while (iter != end)
    {
        try
        {
            ProgressivePage* page = lruCache_->lookup(*iter);

            if (page->regions.numBytes() == pageSize_)
            {
                requestPages.erase(iter++);
            }
            else
            {
                ++iter;
            }
        } catch (NoSuchEntry& e)
        {
            // Lookup failed, increment to next entry
            iter++;
        }
    }
}

void ProgressivePagedMiddlewareCache::updateCache(const std::set<Key>& updatePages,
                                                  bool updatesDirty,
                                                  std::vector<WritebackPage>& outWriteBacks)
{
    set<Key>::const_iterator iter;
    for (iter = updatePages.begin(); iter != updatePages.end(); iter++)
    {
        // Create the cache entry
        DirtyFileRegion fullRegion(iter->key * pageSize_,
                                   pageSize_,
                                   updatesDirty);
        ProgressivePage newEntry;
        newEntry.id = iter->key;
        newEntry.regions.insert(fullRegion);

        try {
            // If the updates are dirty, then insert them into the cache
            // otherwise, if an existing entry isn't already dirty then
            //   insert the entry into the cache
            Key evictedKey(Filename("/"), 0);
            ProgressivePage* evictedValue = 0;
            bool isEvictedDirty = false;

            // Insert the entry
            lruCache_->insertPageAndRecall(*iter, newEntry, updatesDirty,
                                           evictedKey, evictedValue, isEvictedDirty);

            // Only add writeback if the eviction is dirty
            if (isEvictedDirty)
            {
                WritebackPage w = {evictedKey.filename,
                                   evictedKey.key,
                                   evictedValue->regions};
                outWriteBacks.push_back(w);
            }
            else
            {
                delete evictedValue;
            }
        } catch (const NoSuchEntry& e)
        {
            // No eviction was necessary
        }
    }
}

void ProgressivePagedMiddlewareCache::updateCache(spfsMPIFileWriteAtRequest* write,
                                                  vector<WritebackPage>& outWriteBacks)
{
    // Get the set of page ids for this write
    FilePageUtils& utils = FilePageUtils::instance();
    FSOffset offset = write->getOffset();
    FSSize size = write->getCount() * write->getDataType()->getExtent();
    FileDescriptor* fd = write->getFileDes();
    set<FilePageId> pageIds = utils.determineRequestPages(pageSize_,
                                                          offset,
                                                          size,
                                                          fd->getFileView());

    set<FilePageId>::const_iterator end = pageIds.end();
    for (set<FilePageId>::const_iterator iter = pageIds.begin() ; iter != end; ++iter)
    {
        FileRegionSet frs = utils.determinePartialPageRegions(pageSize_,
                                                              *iter,
                                                              offset,
                                                              size,
                                                              fd->getFileView());
        DirtyFileRegionSet dirtyRegions(frs, true);

        // Create the cache entry
        Key newKey(fd->getFilename(), *iter);
        ProgressivePage newPage = {*iter, dirtyRegions};
        try {
            // If the updates are dirty, then insert them into the cache
            // otherwise, if an existing entry isn't already dirty then
            //   insert the entry into the cache
            Key evictedKey(Filename("/"), 0);
            ProgressivePage* evictedValue = 0;
            bool isEvictedDirty = false;

            // Insert the entry
            lruCache_->insertPageAndRecall(newKey, newPage, true,
                                           evictedKey, evictedValue, isEvictedDirty);

            // Only add writeback if the eviction is dirty
            if (isEvictedDirty)
            {
                WritebackPage w = {evictedKey.filename,
                                   evictedKey.key,
                                   evictedValue->regions};
                outWriteBacks.push_back(w);
            }
            else
            {
                delete evictedValue;
            }
        } catch (const NoSuchEntry& e)
        {
            // No eviction was necessary
        }
    }
}

void ProgressivePagedMiddlewareCache::flushCache(const Filename& flushName)
{
    if (0 == (*openFileCounts_)[flushName])
    {
        FilePageFilter filter(flushName);
        vector<ProgressivePage*> flushEntries = lruCache_->getFilteredEntries(filter);
        vector<ProgressivePage*>::const_iterator iter = flushEntries.begin();
        vector<ProgressivePage*>::const_iterator end = flushEntries.end();
        while (iter != end)
        {
            try
            {
                Key removeKey(flushName, (*iter)->id);
                lruCache_->remove(removeKey);
                delete *iter;
                iter++;
            }
            catch(NoSuchEntry& nse)
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "No such entry while flushing a file from the cache: "
                     << flushName << " " << (*iter)->id << endl;
                assert(0);
            }
        }
    }
}

void ProgressivePagedMiddlewareCache::completeRequests()
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
            resp = new spfsMPIFileCloseResponse(0, SPFS_MPI_FILE_CLOSE_RESPONSE);
            resp->setContextPointer(req);
        }

        // Ensure we send the application response back to the application
        // that actually originated the response
        sendApplicationResponse(0.0, resp);
    }
}

void ProgressivePagedMiddlewareCache::registerPendingReadPages(
    spfsMPIFileRequest* fileRequest,
    const set<Key>& readPages)
{
    PendingData& inProcess = (*pendingRequests_)[fileRequest];
    inProcess.readPages.insert(readPages.begin(), readPages.end());
}

void ProgressivePagedMiddlewareCache::registerPendingWriteRequests(
    spfsMPIFileRequest* fileRequest,
    const vector<spfsMPIFileWriteAtRequest*>& pendingWrites)
{
    PendingData& inProcess = (*pendingRequests_)[fileRequest];

    // Add requests
    vector<spfsMPIFileWriteAtRequest*>::const_iterator writeBegin = pendingWrites.begin();
    vector<spfsMPIFileWriteAtRequest*>::const_iterator writeEnd = pendingWrites.end();
    while (writeBegin != writeEnd)
    {
        inProcess.writeRequests.insert(*(writeBegin++));
    }
}

void ProgressivePagedMiddlewareCache::resolvePendingPages(
    const set<Key>& resolvedPages)
{
    RequestMap::iterator iter;
    for (iter = pendingRequests_->begin(); pendingRequests_->end() != iter; ++iter)
    {
        set<Key> result;
        set<Key>& inProcessReads = iter->second.readPages;
        set_difference(inProcessReads.begin(), inProcessReads.end(),
                       resolvedPages.begin(), resolvedPages.end(),
                       inserter(result, result.begin()));

        // Assign the resulting set as the new pending requests
        inProcessReads = result;
    }
}

void ProgressivePagedMiddlewareCache::resolvePendingRequest(spfsMPIFileRequest* completedRequest)
{
    RequestMap::iterator iter;
    for (iter = pendingRequests_->begin(); pendingRequests_->end() != iter; ++iter)
    {
        iter->second.writeRequests.erase(completedRequest);
    }
}

vector<spfsMPIFileRequest*> ProgressivePagedMiddlewareCache::popCompletedRequests()
{
    vector<spfsMPIFileRequest*> completeRequests;

    // Iterate through the requests to find completed requests
    RequestMap::iterator iter;
    for (iter = pendingRequests_->begin(); pendingRequests_->end() != iter; /* Do Nothing */)
    {
        RequestMap::iterator currentEle = iter++;
        PendingData& inProcess = currentEle->second;
        if (0 != currentEle->first &&
            inProcess.readPages.size() == 0 &&
            inProcess.writeRequests.size() == 0)
        {
            // Remove completed requests
            if (0 == dynamic_cast<spfsMPIFileCloseRequest*>(currentEle->first))
            {
                completeRequests.push_back(currentEle->first);
                pendingRequests_->erase(currentEle->first);
            }
            else
            {
                // If this is a close, we need to additionally ensure that
                // no other reads or writes for this file are ongoing
                Filename closeName = currentEle->first->getFileDes()->getFilename();
                if (!hasPendingData(closeName))
                {
                    completeRequests.push_back(currentEle->first);
                    pendingRequests_->erase(currentEle);
                }
            }
        }
    }
    return completeRequests;
}

bool ProgressivePagedMiddlewareCache::hasPendingData(const Filename& filename) const
{
    RequestMap::const_iterator requestIter;
    RequestMap::const_iterator requestMapEnd = pendingRequests_->end();
    for (requestIter = pendingRequests_->begin(); requestIter != requestMapEnd; requestIter++)
    {
        const set<Key>& pendingReads = requestIter->second.readPages;
        set<Key>::const_iterator iter = pendingReads.begin();
        set<Key>::const_iterator end = pendingReads.end();
        while (iter != end)
        {
            if (filename == iter->filename)
            {
                return true;
            }
            ++iter;
        }

        const set<spfsMPIFileRequest*>& pendingWrites = requestIter->second.writeRequests;
        set<spfsMPIFileRequest*>::const_iterator iter2 = pendingWrites.begin();
        set<spfsMPIFileRequest*>::const_iterator end2 = pendingWrites.end();
        while (iter2 != end2)
        {
            spfsMPIFileRequest* request = *iter2;
            if (filename == request->getFileDes()->getFilename())
            {
                return true;
            }
            ++iter2;
        }
    }
    return false;
}

set<ProgressivePagedMiddlewareCache::Key>
ProgressivePagedMiddlewareCache::trimPendingReadPages(const set<Key>& pageKeys)
{
    if (!pendingRequests_->empty())
    {
        set<Key> result;
        RequestMap::const_iterator iter;
        for (iter = pendingRequests_->begin(); pendingRequests_->end() != iter; ++iter)
        {
            const PendingData& inProcess = iter->second;
            set_difference(pageKeys.begin(), pageKeys.end(),
                           inProcess.readPages.begin(), inProcess.readPages.end(),
                           inserter(result, result.begin()));
        }
        // Return the resulting set as the new set of trimmed pages
        return result;
    }
    return pageKeys;
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
