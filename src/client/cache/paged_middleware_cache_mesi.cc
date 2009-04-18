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
#include "paged_middleware_cache_mesi.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include "cache_proto_m.h"
#include "comm_man.h"
#include "file_builder.h"
#include "mpi_proto_m.h"
using namespace std;

// OMNet Registration Method
Define_Module(PagedMiddlewareCacheMesi);

/** Convenience typedef */
typedef LRUMesiCache<PagedCache::Key, FilePageId> MesiCacheType;

/** Functor for finding dirty pages for a cached file */
class MesiModifiedPageFilter : public LRUMesiCache<PagedCache::Key,
                                                   FilePageId>::FilterFunctor
{
public:
    /** Constructor */
    MesiModifiedPageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file and is dirty */
    virtual bool filter(const PagedCache::Key& key,
                        const FilePageId& entry,
                        MesiCacheType::State state) const
    {
        return ((key.filename == filename_) &&
                (MesiCacheType::MODIFIED == state));
    };

private:
    Filename filename_;
};

/** Functor for finding dirty pages for a cached file */
class MesiFilePageFilter : public LRUMesiCache<PagedCache::Key,
                                               FilePageId>::FilterFunctor
{
public:
    /** Constructor */
    MesiFilePageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file and is dirty */
    virtual bool filter(const PagedCache::Key& key,
                        const FilePageId& entry,
                        MesiCacheType::State state) const
    {
        return (key.filename == filename_);
    };

private:
    Filename filename_;
};

/** Name used for writeback messages */
const string PagedMiddlewareCacheMesi::PAGE_WRITEBACK_NAME("Page Writeback");

PagedMiddlewareCacheMesi::PagedMiddlewareCacheMesi()
    : lruCache_(0),
      pendingPages_(0),
      openFileCounts_(0)
{
}

void PagedMiddlewareCacheMesi::initialize()
{
    // Initialize parents
    PagedCache::initialize();
    PageAccessMixin::setPageSize(PagedCache::pageSize());

    // Initialize LRU management
    assert(0 != cacheCapacity());
    lruCache_ = createFileDataPageCache(cacheCapacity());

    // Initialize the pending request map
    pendingPages_ = createPendingPageMap();

    // Initialize the open file map
    openFileCounts_ = createOpenFileMap();
}

PagedMiddlewareCacheMesi::FileDataPageCache*
PagedMiddlewareCacheMesi::createFileDataPageCache(size_t cacheSize)
{
    return new FileDataPageCache(cacheSize);
}

PagedMiddlewareCacheMesi::RequestMap*
PagedMiddlewareCacheMesi::createPendingPageMap()
{
    return new RequestMap();
}

PagedMiddlewareCacheMesi::OpenFileMap*
PagedMiddlewareCacheMesi::createOpenFileMap()
{
    return new OpenFileMap();
}

void PagedMiddlewareCacheMesi::performFakeOpen(const Filename& openName)
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


void PagedMiddlewareCacheMesi::handleApplicationMessage(cMessage* msg)
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

void PagedMiddlewareCacheMesi::handleFileSystemMessage(cMessage* msg)
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
            spfsMPIFileRequest* fileRequest =
                dynamic_cast<spfsMPIFileRequest*>(cacheRequest);
            assert(0 != fileRequest);
            delete fileRequest->getFileDes();
            delete fileRequest;
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

void PagedMiddlewareCacheMesi::processRequest(cMessage* request, cMessage* msg)
{
    if (0 == request)
    {
        // Extract the writeback pages and resolve the associated pages
        assert(0 != dynamic_cast<spfsMPIFileWriteAtResponse*>(msg));
        spfsMPIFileWriteAtRequest* writeback =
            static_cast<spfsMPIFileWriteAtRequest*>(msg->contextPointer());

        // Resolve this set of pending pages
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

void PagedMiddlewareCacheMesi::processFileOpen(spfsMPIFileOpenRequest* open,
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

void PagedMiddlewareCacheMesi::processFileClose(spfsMPIFileCloseRequest* close, cMessage* msg)
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
            //cerr << "Last close: Evictions are on" << endl;
            set<PagedCache::Key> dirtyPages = lookupModifiedPagesInCache(closeName);
            flushCache(closeName);
            registerPendingWritePages(close, dirtyPages);
            beginWritebackEvictions(dirtyPages, close);
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

        // Resolve the flushed pages
        set<PagedCache::Key> flushPages;
        getRequestCachePages(flushReq, flushPages);
        resolvePendingWritePages(flushPages);
    }
}

void PagedMiddlewareCacheMesi::processFileRead(spfsMPIFileReadAtRequest* read, cMessage* msg)
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
        beginReadShared(trimmedPages, read);

        // Register the request's pending pages
        set<PagedCache::Key> readPages;
        registerPendingPages(read, requestPages, readPages);
    }
    else
    {
        assert(0 != dynamic_cast<spfsCacheReadSharedResponse*>(msg));

        // Determine the set of read pages
        spfsMPIFileReadAtRequest* cacheRead =
            static_cast<spfsMPIFileReadAtRequest*>(msg->contextPointer());

        // Determine the cache pages read
        set<PagedCache::Key> requestPages;
        getRequestCachePages(cacheRead, requestPages);

        // Update the cache
        set<PagedCache::Key> writebackPages;
        updateCacheWithReadPages(requestPages, writebackPages);

        // Update the pending requests
        resolvePendingReadPages(requestPages);

        beginWritebackEvictions(writebackPages, 0);

        // TODO: If the writebuffer is not infinite, the request will need
        // to pause while writebacks occur
        //addPendingWrites(read, writebackPages);
    }
}

void PagedMiddlewareCacheMesi::processFileWrite(spfsMPIFileWriteAtRequest* write, cMessage* msg)
{
    // Cache write state machine states
    enum {
        INIT = 0,
        BEGIN_CACHE_BYPASS_WRITE = FSM_Steady(1),
        COMPLETE_CACHE_BYPASS_WRITE = FSM_Steady(2),
        UPDATE_CACHED_PAGES = FSM_Transient(3),
        BEGIN_UPGRADE_PAGE_READ = FSM_Steady(4),
        COUNT_UPGRADE_ARRIVALS = FSM_Steady(5),
        UPDATE_CACHE_AND_WRITEBACK = FSM_Steady(6),
        COMPLETE_WRITE = FSM_Steady(7)
    };

    // Variables needed for multiple states
    set<PagedCache::Key> readExclusivePages;

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
                // TODO this technically allows both read shared and read
                // exclusive requests to satisfy, which is fine for Flash-IO
                // Get exclusive access on the necessary pages
                trimExclusiveCachePages(requestPages);
                readExclusivePages = trimPendingReadPages(requestPages);

                // Register the request for completion
                set<PagedCache::Key> noPages;
                registerPendingPages(write, readExclusivePages, noPages);

                if (readExclusivePages.empty())
                {
                    // Only need to update cached pages
                    FSM_Goto(currentState, UPDATE_CACHED_PAGES);
                }
                else
                {
                    FSM_Goto(currentState, BEGIN_UPGRADE_PAGE_READ);
                }
            }
            break;
        }
        case FSM_Enter(BEGIN_CACHE_BYPASS_WRITE):
        {
            assert(write == msg);
            cerr << "Bypass write" << endl;
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
        case FSM_Enter(UPDATE_CACHED_PAGES):
        {
            // Add the delay for reading the exclusive cache pages
            // and writing the new data to memory
            double readBytes = readExclusivePages.size() * pageSize();
            double writeBytes = write->getCount() * write->getDataType()->getExtent();
            double delay = (readBytes + writeBytes) * byteCopyTime();
            cPar* delayPar = new cPar("Delay");
            delayPar->setDoubleValue(delay);
            write->addPar(delayPar);

            break;
        }
        case FSM_Exit(UPDATE_CACHED_PAGES):
        {
            cerr << "Cache write" << endl;
            // Transition to next state
            if (readExclusivePages.empty())
            {
                FSM_Goto(currentState, COMPLETE_WRITE);
            }
            else
            {
                FSM_Goto(currentState, BEGIN_UPGRADE_PAGE_READ);
            }
            break;
        }
        case FSM_Enter(BEGIN_UPGRADE_PAGE_READ):
        {
            // Begin the read
            beginReadExclusive(readExclusivePages, write);
            break;
        }
        case FSM_Exit(BEGIN_UPGRADE_PAGE_READ):
        {
            // Transition to next state
            if (readExclusivePages.empty())
            {
                FSM_Goto(currentState, UPDATE_CACHE_AND_WRITEBACK);
            }
            else
            {
                FSM_Goto(currentState, COUNT_UPGRADE_ARRIVALS);
            }
            break;
        }
        case FSM_Enter(COUNT_UPGRADE_ARRIVALS):
        {
            break;
        }
        case FSM_Exit(COUNT_UPGRADE_ARRIVALS):
        {
            //countPageArrivals();
            spfsCacheReadExclusiveRequest* readRequest = 0;
            if (0 == readRequest->getRemainingPages())
            {
                FSM_Goto(currentState, UPDATE_CACHE_AND_WRITEBACK);
            }
            else
            {
                FSM_Goto(currentState, COUNT_UPGRADE_ARRIVALS);
            }
            break;
        }
        case FSM_Enter(UPDATE_CACHE_AND_WRITEBACK):
        {
            assert(0 != dynamic_cast<spfsMPIFileReadAtResponse*>(msg));

            // First insert the just read pages into the cache
            set<PagedCache::Key> writebackPages;
            spfsMPIFileReadAtRequest* read =
                static_cast<spfsMPIFileReadAtRequest*>(msg->contextPointer());
            set<PagedCache::Key> readPages;
            getRequestCachePages(read, readPages);
            updateCacheWithReadPages(readPages, writebackPages);
            resolvePendingReadPages(readPages);

            // TODO: If the writebuffer is not infinite, the request will need
            // to pause while writebacks occur
            //registerPendingWritePages(write, writebackPages);
            beginWritebackEvictions(writebackPages, 0);
            break;
        }
        case FSM_Exit(UPDATE_CACHE_AND_WRITEBACK):
        {
            // Need to stay in this state, the read may take multiple
            // messages to finish
            break;
        }
        case FSM_Enter(COMPLETE_WRITE):
        {
            // No-op used only when there is no read required
            break;
        }
        case FSM_Exit(COMPLETE_WRITE):
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Illegal state reached: " << msg->info() << endl;
            assert(0);
            break;
        }
    }

    // Set the cache fsm state back into the request
    write->setCacheState(currentState);
}

template<class spfsMPIFileIORequest>
void PagedMiddlewareCacheMesi::getRequestCachePages(
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

void PagedMiddlewareCacheMesi::getRequestCachePages(
    const spfsMPIFileWriteAtRequest* ioRequest,
    vector<PagedCache::Key>& outFullPages,
    vector<PagedCache::Key>& outPartialPages) const
{
    // Convert regions into file pages
    FileDescriptor* fd = ioRequest->getFileDes();
    FSSize ioSize = ioRequest->getDataType()->getExtent() * ioRequest->getCount();

    // Construct the pages for full regions
    set<FilePageId> fullPages =
        determineRequestFullPages(ioRequest->getOffset(),
                                  ioSize,
                                  fd->getFileView());
    set<FilePageId>::const_iterator first = fullPages.begin();
    set<FilePageId>::const_iterator last = fullPages.end();
    while (first != last)
    {
        PagedCache::Key page(fd->getFilename(), *first);
        outFullPages.push_back(page);
        first++;
    }

    // Construct the pages for partial regions
    set<FilePageId> partialPages =
        determineRequestPartialPages(ioRequest->getOffset(),
                                     ioSize,
                                     fd->getFileView());
    first = partialPages.begin();
    last = partialPages.end();
    while (first != last)
    {
/*        MultiCache::PartialPage page;
        page.id = *first;
        page.regions = determinePartialPageRegions(page.id,
                                                   ioRequest->getOffset(),
                                                   ioSize,
                                                   fd->getFileView());
        outPartialPages.push_back(page);
*/        first++;
    }
}

template<class spfsMPIFileIORequest>
void PagedMiddlewareCacheMesi::getRequestPartialCachePages(
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


void PagedMiddlewareCacheMesi::beginWritebackEvictions(
    const set<PagedCache::Key>& writebackPages,
    spfsMPIFileRequest* parentRequest)
{
    vector<spfsMPIFileWriteAtRequest*> writebacks;
    writebacks = createPFSWriteRequests(writebackPages, parentRequest);
    for (size_t i = 0; i < writebacks.size(); i++)
    {
        send(writebacks[i], fsOutGateId());
    }
}

void PagedMiddlewareCacheMesi::beginReadExclusive(
    const std::set<PagedCache::Key>& readPages,
    spfsMPIFileRequest* parentRequest)
{
    assert(0 != readPages.size());
    // Construct and dispatch the requests
    vector<spfsCacheReadExclusiveRequest*> readRequests =
        createCacheReadExclusiveRequests(getRank(), readPages, parentRequest);
    for (size_t i = 0; i < readRequests.size(); i++)
    {
        send(readRequests[i], fsOutGateId());
    }
}

void PagedMiddlewareCacheMesi::beginReadShared(
    const std::set<PagedCache::Key>& readPages,
    spfsMPIFileRequest* parentRequest)
{
    assert(0 != readPages.size());
    // Construct and dispatch the requests
    vector<spfsCacheReadSharedRequest*> readRequests =
        createCacheReadSharedRequests(getRank(), readPages, parentRequest);
    for (size_t i = 0; i < readRequests.size(); i++)
    {
        send(readRequests[i], fsOutGateId());
    }
}

set<PagedCache::Key>
PagedMiddlewareCacheMesi::lookupModifiedPagesInCache(const Filename& filename) const
{
    MesiModifiedPageFilter filter(filename);
    return lruCache_->getFilteredKeys(filter);
}

set<PagedCache::Key>
PagedMiddlewareCacheMesi::lookupPagesInCache(set<PagedCache::Key>& requestPages)
{
    set<PagedCache::Key> cachedPages;
    set<PagedCache::Key>::iterator iter = requestPages.begin();
    set<PagedCache::Key>::iterator end = requestPages.end();
    while (iter != end)
    {
        try
        {
            PagedCache::Key key(iter->filename, iter->key);
            lruCache_->lookup(key);
            cachedPages.insert(*iter);
            requestPages.erase(iter++);
        } catch (NoSuchEntry& e)
        {
            // Lookup failed, increment to next entry
            iter++;
        }
    }
    return cachedPages;
}

void PagedMiddlewareCacheMesi::trimExclusiveCachePages(
    set<PagedCache::Key>& requestPages)
{
    set<PagedCache::Key>::iterator iter = requestPages.begin();
    set<PagedCache::Key>::iterator end = requestPages.end();
    while (iter != end)
    {
        try
        {
            PagedCache::Key key(iter->filename, iter->key);
            MesiCacheType::State state = lruCache_->lookupState(key);

            if (MesiCacheType::EXCLUSIVE == state ||
                MesiCacheType::MODIFIED == state)
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

void PagedMiddlewareCacheMesi::updateCacheWithReadPages(set<PagedCache::Key>& updatePages,
                                                        set<PagedCache::Key>& outWriteBacks)
{
    // Update the cache and accumulate any writebacks required
    set<PagedCache::Key>::const_iterator iter;
    for (iter = updatePages.begin(); iter != updatePages.end(); iter++)
    {
        try {
            // Create an entry for the clean page even if the dirty page
            // already exists
            PagedCache::Key evictedKey(Filename("/"), 0);
            FilePageId evictedPage = 0;
            MesiCacheType::State evictedState = MesiCacheType::NULL_STATE;
            lruCache_->insertAndRecall(*iter, iter->key, MesiCacheType::SHARED,
                                       evictedKey,
                                       evictedPage,
                                       evictedState);

            // Only add writeback if the eviction is dirty
            if (MesiCacheType::MODIFIED == evictedState)
            {
                outWriteBacks.insert(evictedKey);
            }
        } catch (const NoSuchEntry& e)
        {
            // No eviction was necessary
        }
    }
}


void PagedMiddlewareCacheMesi::updateCacheWithWritePages(
    const Filename& filename,
    const set<FilePageId>& writePages,
    set<PagedCache::Key>& outWritebacks)
{
    // Update the cache with the write pages and accumulate any writebacks
    set<FilePageId>::const_iterator iter = writePages.begin();
    set<FilePageId>::const_iterator last = writePages.end();
    while (iter != last)
    {
        PagedCache::Key key(filename, *iter);
        PagedCache::Key evictedKey(Filename("/"), 0);
        FilePageId evictedPage = 0;
        MesiCacheType::State evictedState = MesiCacheType::NULL_STATE;
        try {
            lruCache_->insertAndRecall(key, *iter, MesiCacheType::MODIFIED,
                                       evictedKey,
                                       evictedPage,
                                       evictedState);

            // Add the writeback entry
            if (MesiCacheType::MODIFIED == evictedState)
            {
                outWritebacks.insert(evictedKey);
            }
        } catch(NoSuchEntry& e)
        {
            // No eviction occurred
        }
        ++iter;
    }
}

void PagedMiddlewareCacheMesi::flushCache(const Filename& flushName)
{
    if (0 == (*openFileCounts_)[flushName])
    {
        MesiFilePageFilter filter(flushName);
        set<PagedCache::Key> pageIds = lruCache_->getFilteredKeys(filter);
        set<PagedCache::Key>::const_iterator pageBegin = pageIds.begin();
        set<PagedCache::Key>::const_iterator pageEnd = pageIds.end();
        while (pageBegin != pageEnd)
        {
            lruCache_->remove(*pageBegin);
            pageBegin++;
        }
    }
}

void PagedMiddlewareCacheMesi::countPageArrivals(spfsCacheReadRequest* read,
                                                 cMessage* response)
{
    assert(false);
}

void PagedMiddlewareCacheMesi::completeRequests()
{
    vector<spfsMPIFileRequest*> completedRequests = popCompletedRequests();

    for (size_t i = 0; i < completedRequests.size(); i++)
    {
        spfsMPIResponse* resp = 0;
        spfsMPIFileRequest* req = completedRequests[i];
        double delay = 0.0;
        if (SPFS_MPI_FILE_READ_AT_REQUEST == req->kind())
        {
            resp =
                new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
            resp->setContextPointer(req);
        }
        else if (SPFS_MPI_FILE_WRITE_AT_REQUEST == req->kind())
        {
            delay = req->par("Delay").doubleValue();
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
        sendApplicationResponse(delay, resp);
    }
}

void PagedMiddlewareCacheMesi::registerPendingPages(
    spfsMPIFileRequest* fileRequest,
    const set<PagedCache::Key>& readPages,
    const set<PagedCache::Key>& writePages)
{
    PagedCache::InProcessPages& inProcess = (*pendingPages_)[fileRequest];
    inProcess.readPages.insert(readPages.begin(), readPages.end());
    inProcess.writePages.insert(writePages.begin(), writePages.end());
}

void PagedMiddlewareCacheMesi::registerPendingWritePages(spfsMPIFileRequest* request,
                                                         const set<PagedCache::Key>& pendingWrites)
{
    PagedCache::InProcessPages& inProcess = (*pendingPages_)[request];
    set<PagedCache::Key>::const_iterator writeBegin = pendingWrites.begin();
    set<PagedCache::Key>::const_iterator writeEnd = pendingWrites.end();
    while (writeBegin != writeEnd)
    {
        inProcess.writePages.insert(*writeBegin);
        writeBegin++;
    }
}

void PagedMiddlewareCacheMesi::resolvePendingReadPage(const PagedCache::Key& resolvedPage)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        iter->second.readPages.erase(resolvedPage);
    }
}

void PagedMiddlewareCacheMesi::resolvePendingReadPages(const set<PagedCache::Key>& resolvedPages)
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

void PagedMiddlewareCacheMesi::resolvePendingReadPages(const Filename& filename,
                                                       const set<FilePageId>& readPages)
{
    set<PagedCache::Key> readKeys;
    for (size_t i = 0; i < readPages.size(); i++)
    {
        //TODO PagedCache::Key key(filename, readPages[i].id);
        //TODO readKeys.insert(key);
    }
    resolvePendingReadPages(readKeys);
}

void PagedMiddlewareCacheMesi::resolvePendingWritePage(const PagedCache::Key& resolvedPage)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        iter->second.writePages.erase(resolvedPage);
    }
}

void PagedMiddlewareCacheMesi::resolvePendingWritePages(const set<PagedCache::Key>& resolvedPages)
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

vector<spfsMPIFileRequest*> PagedMiddlewareCacheMesi::popCompletedRequests()
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
                spfsMPIFileRequest* request = currentEle->first;
                Filename closeName = request->getFileDes()->getFilename();
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

set<PagedCache::Key> PagedMiddlewareCacheMesi::trimPendingReadPages(const set<PagedCache::Key>& pageIds)
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

set<PagedCache::Key> PagedMiddlewareCacheMesi::trimPendingWritePages(const set<PagedCache::Key>& pages)
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

bool PagedMiddlewareCacheMesi::hasPendingPages(const Filename& filename) const
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
