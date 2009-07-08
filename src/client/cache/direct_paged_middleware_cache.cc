//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "direct_paged_middleware_cache.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include "comm_man.h"
#include "lru_cache_filters.h"
#include "mpi_proto_m.h"
using namespace std;

// OMNet Registration Method
Define_Module(DirectPagedMiddlewareCache);

DirectPagedMiddlewareCache::DirectPagedMiddlewareCache()
    : lruCache_(0),
      pendingPages_(0),
      openFileCounts_(0)
{
}

void DirectPagedMiddlewareCache::initialize()
{
    // Initialize parent
    PagedCache::initialize();

    // Initialize LRU management
    assert(0 != cacheCapacity());
    lruCache_ = createFileDataPageCache(cacheCapacity());

    // Initialize the pending request maps
    pendingPages_ = createPendingPageMap();

    // Initialize the open file map
    openFileCounts_ = createOpenFileMap();
}

DirectPagedMiddlewareCache::FileDataPageCache*
DirectPagedMiddlewareCache::createFileDataPageCache(size_t cacheSize)
{
    return new FileDataPageCache(cacheSize);
}

DirectPagedMiddlewareCache::RequestMap*
DirectPagedMiddlewareCache::createPendingPageMap()
{
    return new RequestMap();
}

DirectPagedMiddlewareCache::OpenFileMap*
DirectPagedMiddlewareCache::createOpenFileMap()
{
    return new OpenFileMap();
}

void DirectPagedMiddlewareCache::performFakeOpen(const Filename& openName)
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


void DirectPagedMiddlewareCache::handleApplicationMessage(cMessage* msg)
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

void DirectPagedMiddlewareCache::handleFileSystemMessage(cMessage* msg)
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

void DirectPagedMiddlewareCache::processRequest(cMessage* request, cMessage* msg)
{
    if (0 == request)
    {
        // Extract the writeback pages and resolve the associated pages
        assert(0 != dynamic_cast<spfsMPIFileWriteAtResponse*>(msg));
        spfsMPIFileWriteAtRequest* writeback =
            static_cast<spfsMPIFileWriteAtRequest*>(msg->contextPointer());

        // Determine the cache pages requested
        set<PagedCache::Key> writtenPages;
        getRequestCachePages(writeback, writtenPages);
        resolvePendingWritePages(writtenPages);
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

void DirectPagedMiddlewareCache::processFileOpen(spfsMPIFileOpenRequest* open,
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

void DirectPagedMiddlewareCache::processFileClose(spfsMPIFileCloseRequest* close, cMessage* msg)
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
            set<PagedCache::Key> noPages;
            set<PagedCache::Key> writePages = lookupDirtyPagesInCache(closeName);

            beginWritebackEvictions(writePages, close);
            flushCache(closeName);
            registerPendingPages(close, noPages, writePages);
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

        // Resolve the pending pages
        set<PagedCache::Key> flushPages;
        getRequestCachePages(flushReq, flushPages);
        resolvePendingWritePages(flushPages);
    }
}

void DirectPagedMiddlewareCache::processFileRead(spfsMPIFileReadAtRequest* read, cMessage* msg)
{
    if (msg == read)
    {
        // Determine the cache pages requested
        set<PagedCache::Key> requestPages;
        getRequestCachePages(read, requestPages);

        // Remove cached pages
        lookupPagesInCache(requestPages);

        // Cull read pages that are already requested
        set<PagedCache::Key> trimmedPages = trimPendingReadPages(requestPages);

        // Begin the read
        beginRead(trimmedPages, read);

        // Register the request's pending pages
        set<PagedCache::Key> writePages;
        registerPendingPages(read, requestPages, writePages);
    }
    else
    {
        assert(0 != dynamic_cast<spfsMPIFileReadAtResponse*>(msg));

        // Determine the set of read pages
        spfsMPIFileReadAtRequest* cacheRead =
            static_cast<spfsMPIFileReadAtRequest*>(msg->contextPointer());

        // Determine the cache pages read
        set<PagedCache::Key> requestPages;
        getRequestCachePages(cacheRead, requestPages);

        // Update the cache
        set<PagedCache::Key> writebackPages;
        updateCache(requestPages, false, writebackPages);

        // Update the pending requests
        resolvePendingReadPages(requestPages);

        // TODO: If the writebuffer is not infinite, the request will need
        // to pause while writebacks occur
        beginWritebackEvictions(writebackPages, 0);
        //addPendingWrites(read, writebackPages);
    }
}

void DirectPagedMiddlewareCache::processFileWrite(spfsMPIFileWriteAtRequest* write, cMessage* msg)
{
    // Cache write state machine states
    enum {
        INIT = 0,
        BEGIN_CACHE_BYPASS_WRITE = FSM_Steady(1),
        COMPLETE_CACHE_BYPASS_WRITE = FSM_Steady(2),
        BEGIN_PARTIAL_PAGE_READ = FSM_Steady(3),
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
            set<PagedCache::Key> requestPages;
            getRequestCachePages(write, requestPages);

            if (requestPages.size() > lruCache_->capacity())
            {
                FSM_Goto(currentState, BEGIN_CACHE_BYPASS_WRITE);
            }
            else
            {
                // Determine if partial pages need to be read
                set<PagedCache::Key> allPartialPages;
                getRequestPartialCachePages(write, allPartialPages);
                lookupPagesInCache(allPartialPages);
                partialPagesTrimmed = trimPendingReadPages(allPartialPages);

                // Register the request for completion
                set<PagedCache::Key> noPages;
                registerPendingPages(write, allPartialPages, noPages);

                if (!partialPagesTrimmed.empty())
                {
                    FSM_Goto(currentState, BEGIN_PARTIAL_PAGE_READ);
                }
                else
                {
                    FSM_Goto(currentState, UPDATE_CACHE);
                }
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
        case FSM_Enter(BEGIN_PARTIAL_PAGE_READ):
        {
            // Begin the read
            beginRead(partialPagesTrimmed, write);
            break;
        }
        case FSM_Exit(BEGIN_PARTIAL_PAGE_READ):
        {
            // Extract the read pages to resolve them for pending requests
            assert(0 != dynamic_cast<spfsMPIFileReadAtResponse*>(msg));
            spfsMPIFileReadAtRequest* read =
                static_cast<spfsMPIFileReadAtRequest*>(msg->contextPointer());
            set<PagedCache::Key> readPages;
            getRequestCachePages(read, readPages);

            // Should we resolve here, or in the next step?
            resolvePendingReadPages(readPages);

            // Transition to next state
            FSM_Goto(currentState, UPDATE_CACHE);
            break;
        }
        case FSM_Enter(UPDATE_CACHE):
        {
            // FIXME: Some partial pages may still be pending here
            // Update the cache with write data
            set<PagedCache::Key> updatePages;
            getRequestCachePages(write, updatePages);

            // Update the cache and acquire any necessary writebacks
            set<PagedCache::Key> writebackPages;
            updateCache(updatePages, true, writebackPages);

            // Update the pending requests
            resolvePendingReadPages(updatePages);

            // TODO: If the writebuffer is not infinite, the request will need
            // to pause while writebacks occur
            beginWritebackEvictions(writebackPages, 0);
            set<PagedCache::Key> noReadPages;
            registerPendingPages(write, noReadPages, writebackPages);
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
void DirectPagedMiddlewareCache::getRequestCachePages(
    const spfsMPIFileIORequest* ioRequest,
    std::set<PagedCache::Key>& outRequestPages) const
{
    // Convert regions into file pages
    FileDescriptor* fd = ioRequest->getFileDes();
    FSSize ioSize = ioRequest->getDataType()->getExtent() * ioRequest->getCount();
    set<FilePageId> requestPages = determineRequestPages(ioRequest->getOffset(),
                                                         ioSize,
                                                         fd->getFileView());

    outRequestPages = PagedCache::convertPagesToCacheKeys(fd->getFilename(),
                                                          requestPages);
}
template<class spfsMPIFileIORequest>
void DirectPagedMiddlewareCache::getRequestPartialCachePages(
    const spfsMPIFileIORequest* ioRequest,
    std::set<PagedCache::Key>& outRequestPages) const
{
    // Convert regions into file pages
    FileDescriptor* fd = ioRequest->getFileDes();
    FSSize ioSize = ioRequest->getDataType()->getExtent() * ioRequest->getCount();
    set<FilePageId> partialPages =
        determineRequestPartialPages(ioRequest->getOffset(),
                                     ioSize,
                                     fd->getFileView());

    outRequestPages = PagedCache::convertPagesToCacheKeys(fd->getFilename(),
                                                          partialPages);
}


void DirectPagedMiddlewareCache::beginWritebackEvictions(
    const std::set<PagedCache::Key>& writebackPages,
    spfsMPIFileRequest* parentRequest)
{
    if (!writebackPages.empty())
    {
        // Add the writeback as a pending request
        set<PagedCache::Key> noReadPages;
        registerPendingPages(parentRequest, noReadPages, writebackPages);

        // Accumulate the pages for each file and issue a writeback
        set<FilePageId> pages;
        Filename currentName = writebackPages.begin()->filename;
        set<PagedCache::Key>::iterator iter;
        for (iter = writebackPages.begin(); iter != writebackPages.end(); ++iter)
        {
            // TODO: need to ensure that pages are stably sorted by current name
            if (currentName == iter->filename)
            {
                pages.insert(iter->key);
            }
            else
            {
                // Write back the accumulated pages for currentName
                spfsMPIFileWriteAtRequest* writebackRequest =
                    createPageWriteRequest(currentName, pages, parentRequest);
                send(writebackRequest, fsOutGateId());
                // Reset the loop state and append the page to the new set
                pages.clear();
                currentName = iter->filename;
                pages.insert(iter->key);
            }
        }

        // Write back the final set of accumulated pages
        spfsMPIFileWriteAtRequest* writeBackRequest =
            createPageWriteRequest(currentName, pages, parentRequest);
        send(writeBackRequest, fsOutGateId());
    }
}

void DirectPagedMiddlewareCache::beginRead(
    const std::set<PagedCache::Key>& readPages,
    spfsMPIFileRequest* parentRequest)
{
    assert(0 != readPages.size());

    // Collect the page ids
    set<PagedCache::Key>::iterator iter = readPages.begin();
    set<PagedCache::Key>::iterator end = readPages.end();
    Filename filename = iter->filename;
    set<FilePageId> pages;
    while (iter != end)
    {
        assert(filename == iter->filename);
        pages.insert(iter->key);
        ++iter;
    }

    // Construct the request
    spfsMPIFileReadAtRequest* readRequest =
        createPageReadRequest(filename, pages, parentRequest);
    send(readRequest, fsOutGateId());
}

set<PagedCache::Key> DirectPagedMiddlewareCache::lookupDirtyPagesInCache(const Filename& filename) const
{
        DirtyPageFilter filter(filename);
        return lruCache_->getFilteredEntries(filter);
}

void DirectPagedMiddlewareCache::lookupPagesInCache(set<PagedCache::Key>& requestPages)
{
    set<PagedCache::Key>::iterator iter = requestPages.begin();
    set<PagedCache::Key>::iterator end = requestPages.end();
    while (iter != end)
    {
        try
        {
            lruCache_->lookup(*iter);
            requestPages.erase(iter++);
        } catch (NoSuchEntry& e)
        {
            // Lookup failed, increment to next entry
            iter++;
        }
    }
}

void DirectPagedMiddlewareCache::updateCache(set<PagedCache::Key>& updatePages,
                                             bool updatesDirty,
                                             set<PagedCache::Key>& outWriteBacks)
{
    // Update the cache and accumulate any writebacks required
    set<PagedCache::Key>::const_iterator iter;
    for (iter = updatePages.begin(); iter != updatePages.end(); iter++)
    {
        try {
            // If the updates are dirty, then insert them into the cache
            // otherwise, if an existing entry isn't already dirty then
            //   insert the entry into the cache
            PagedCache::Key evictedKey(Filename("/"), 0);
            FilePageId evictedValue = 0;
            bool isEvictedDirty = false;
            if (updatesDirty)
            {
                // Insert dirty entries
                lruCache_->insertAndRecall(*iter, iter->key, true,
                                           evictedKey, evictedValue, isEvictedDirty);
            }
            else if (!lruCache_->exists(*iter) ||
                     false == lruCache_->getDirtyBit(*iter))
            {
                // Insert clean entries that don't conflict with an existing
                // dirty entry
                lruCache_->insertAndRecall(*iter, iter->key, false,
                                           evictedKey, evictedValue, isEvictedDirty);
            }

            // Only add writeback if the eviction is dirty
            if (isEvictedDirty)
            {
                outWriteBacks.insert(evictedKey);
            }
        } catch (const NoSuchEntry& e)
        {
            // No eviction was necessary
        }
    }
}

void DirectPagedMiddlewareCache::flushCache(const Filename& flushName)
{
    if (0 == (*openFileCounts_)[flushName])
    {
        FilePageFilter filter(flushName);
        set<PagedCache::Key> flushEntries = lruCache_->getFilteredEntries(filter);
        set<PagedCache::Key>::const_iterator begin = flushEntries.begin();
        set<PagedCache::Key>::const_iterator end = flushEntries.end();
        while (begin != end)
        {
            try
            {
                lruCache_->remove(*begin);
                begin++;
            }
            catch(NoSuchEntry& nse)
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "No such entry while flushing a file from the cache: "
                     << begin->filename << " " << begin->key << endl;
                assert(0);
            }
        }
    }
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
            resp = new spfsMPIFileCloseResponse(0, SPFS_MPI_FILE_CLOSE_RESPONSE);
            resp->setContextPointer(req);
        }

        // Ensure we send the application response back to the application
        // that actually originated the response
        sendApplicationResponse(0.0, resp);
    }
}

void DirectPagedMiddlewareCache::registerPendingPages(
    spfsMPIFileRequest* fileRequest,
    const set<PagedCache::Key>& readPages,
    const set<PagedCache::Key>& writePages)
{
    PagedCache::InProcessPages& inProcess = (*pendingPages_)[fileRequest];
    inProcess.readPages.insert(readPages.begin(), readPages.end());
    inProcess.writePages.insert(writePages.begin(), writePages.end());
}

void DirectPagedMiddlewareCache::resolvePendingReadPage(const PagedCache::Key& resolvedPage)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        iter->second.readPages.erase(resolvedPage);
    }
}

void DirectPagedMiddlewareCache::resolvePendingReadPages(const set<PagedCache::Key>& resolvedPages)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        set<PagedCache::Key> result;
        set<PagedCache::Key>& inProcessReads = iter->second.readPages;
        set_difference(inProcessReads.begin(), inProcessReads.end(),
                       resolvedPages.begin(), resolvedPages.end(),
                       inserter(result, result.begin()));

        // Assign the resulting set as the new pending requests
        inProcessReads = result;
    }
}

void DirectPagedMiddlewareCache::resolvePendingWritePage(const PagedCache::Key& resolvedPage)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        iter->second.writePages.erase(resolvedPage);
    }
}

void DirectPagedMiddlewareCache::resolvePendingWritePages(const set<PagedCache::Key>& resolvedPages)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        set<PagedCache::Key> result;
        set<PagedCache::Key>& inProcessWrites = iter->second.writePages;
        set_difference(inProcessWrites.begin(), inProcessWrites.end(),
                       resolvedPages.begin(), resolvedPages.end(),
                       inserter(result, result.begin()));

        // Assign the resulting set as the new pending requests
        inProcessWrites = result;
    }
}

vector<spfsMPIFileRequest*> DirectPagedMiddlewareCache::popCompletedRequests()
{
    vector<spfsMPIFileRequest*> completeRequests;

    // Iterate through the requests to find completed requests
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; /* Do Nothing */)
    {
        RequestMap::iterator currentEle = iter++;
        PagedCache::InProcessPages& inProcess = currentEle->second;
        if (0 != currentEle->first &&
            inProcess.readPages.size() == 0 &&
            inProcess.writePages.size() == 0)
        {
            // Remove completed requests
            if (0 == dynamic_cast<spfsMPIFileCloseRequest*>(currentEle->first))
            {
                completeRequests.push_back(currentEle->first);
                pendingPages_->erase(currentEle->first);
            }
            else
            {
                // If this is a close, we need to additionally ensure that
                // no other reads or writes for this file are ongoing
                Filename closeName = currentEle->first->getFileDes()->getFilename();
                if (!hasPendingPages(closeName))
                {
                    completeRequests.push_back(currentEle->first);
                    pendingPages_->erase(currentEle);
                }
            }
        }
    }
    return completeRequests;
}

set<PagedCache::Key> DirectPagedMiddlewareCache::trimPendingReadPages(const set<PagedCache::Key>& pageIds)
{
    if (!pendingPages_->empty())
    {
        set<PagedCache::Key> result;
        RequestMap::const_iterator iter;
        for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
        {
            const PagedCache::InProcessPages& inProcess = iter->second;
            set_difference(pageIds.begin(), pageIds.end(),
                           inProcess.readPages.begin(), inProcess.readPages.end(),
                           inserter(result, result.begin()));
        }
        // Return the resulting set as the new set of trimmed pages
        return result;
    }
    return pageIds;
}

set<PagedCache::Key> DirectPagedMiddlewareCache::trimPendingWritePages(const set<PagedCache::Key>& pages)
{
    if (!pendingPages_->empty())
    {
        set<PagedCache::Key> result;
        RequestMap::const_iterator iter;
        for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
        {
            const PagedCache::InProcessPages& inProcess = iter->second;
            set_difference(pages.begin(), pages.end(),
                           inProcess.writePages.begin(), inProcess.writePages.end(),
                           inserter(result, result.begin()));
        }
        // Return the resulting set as the new set of trimmed pages
        return result;
    }
    return pages;
}

bool DirectPagedMiddlewareCache::hasPendingPages(const Filename& filename) const
{
    RequestMap::const_iterator requestIter;
    RequestMap::const_iterator requestMapEnd = pendingPages_->end();;
    for (requestIter = pendingPages_->begin(); requestIter != requestMapEnd; requestIter++)
    {
        const set<PagedCache::Key>& pendingReads = requestIter->second.readPages;
        set<PagedCache::Key>::const_iterator iter = pendingReads.begin();
        set<PagedCache::Key>::const_iterator end = pendingReads.end();
        while (iter != end)
        {
            if (filename == iter->filename)
            {
                return true;
            }
            ++iter;
        }

        const set<PagedCache::Key>& pendingWrites = requestIter->second.writePages;
        iter = pendingWrites.begin();
        end = pendingWrites.end();
        while (iter != end)
        {
            if (filename == iter->filename)
            {
                return true;
            }
            ++iter;
        }
    }
    return false;
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
