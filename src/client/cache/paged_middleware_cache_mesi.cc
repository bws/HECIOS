//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "paged_middleware_cache_mesi.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include "cache_proto_m.h"
#include "client_cache_directory.h"
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
        SPFS_MPI_FILE_WRITE_AT_RESPONSE == msg->kind() ||
        SPFS_CACHE_READ_EXCLUSIVE_RESPONSE == msg->kind() ||
        SPFS_CACHE_READ_SHARED_RESPONSE == msg->kind())
    {
        cMessage* cacheRequest = static_cast<cMessage*>(msg->contextPointer());
        cMessage* appRequest = static_cast<cMessage*>(cacheRequest->contextPointer());
        processRequest(appRequest, msg);

        // Don't delete bypass writes
        if (appRequest != cacheRequest)
        {
            cleanupRequest(cacheRequest);
            delete msg;
        }
    }
    else
    {
        assert(SPFS_MPI_FILE_READ_RESPONSE != msg->kind());
        assert(SPFS_MPI_FILE_WRITE_RESPONSE != msg->kind());
        assert(SPFS_CACHE_READ_EXCLUSIVE_RESPONSE != msg->kind());
        assert(SPFS_CACHE_READ_SHARED_RESPONSE != msg->kind());
        assert(SPFS_CACHE_INVALIDATE_RESPONSE != msg->kind());
        assert(SPFS_CACHE_SEND_PAGES != msg->kind());

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

        // Update the directory that these pages are no longer exclusive/modified
        clearDirectoryEntries(writtenPages);
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

void PagedMiddlewareCacheMesi::cleanupRequest(cMessage* request)
{
    assert(0 != request);
    if (spfsMPIFileRequest* fileRequest = dynamic_cast<spfsMPIFileRequest*>(request))
    {
        delete fileRequest->getFileDes();
        delete fileRequest;
    }
    else
    {
        spfsCacheRequest* cacheRequest = dynamic_cast<spfsCacheRequest*>(request);
        assert(0 != cacheRequest);
        delete cacheRequest->getDescriptor();
        delete cacheRequest;
    }
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
            set<PagedCache::Key> dirtyPages = lookupModifiedPagesInCache(closeName);
            flushCache(closeName);
            registerPendingWritePages(close, dirtyPages);
            beginWritebackEvictions(dirtyPages, close);

            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "Final file close: Eviction count: " << dirtyPages.size() << endl;
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

        // Add the wriebacks to the list of pending pages for this request
        registerPendingWritePages(read, writebackPages);
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
        UPDATE_CACHE_AND_WRITEBACK = FSM_Steady(6),
        COMPLETE_WRITE = FSM_Steady(7)
    };

    // Variables needed for multiple states
    set<PagedCache::Key> cacheExclusivePages;
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
                // Get exclusive access on the necessary pages
                trimExclusiveCachePages(requestPages, cacheExclusivePages);
                // TODO this technically allows both read shared and read
                // exclusive requests to satisfy, which is safe for Flash-IO
                readExclusivePages = trimPendingReadPages(requestPages);

                // Register the request for completion
                set<PagedCache::Key> noPages;
                registerPendingPages(write, readExclusivePages, noPages);
                FSM_Goto(currentState, UPDATE_CACHED_PAGES);
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
        case FSM_Enter(UPDATE_CACHED_PAGES):
        {
            // Update the pages already in the cache
            updateCacheExclusivePages(cacheExclusivePages);

            // Figure out the memory copy delay for the read and write
            double readBytes = readExclusivePages.size() * pageSize();
            double writeBytes = write->getCount() * write->getDataType()->getExtent();
            double delay = (readBytes + writeBytes) * byteCopyTime();
            addCacheMemoryDelay(write, delay);
            break;
        }
        case FSM_Exit(UPDATE_CACHED_PAGES):
        {
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
            FSM_Goto(currentState, UPDATE_CACHE_AND_WRITEBACK);
            break;
        }
        case FSM_Enter(UPDATE_CACHE_AND_WRITEBACK):
        {
            assert(0 != dynamic_cast<spfsCacheReadExclusiveResponse*>(msg));

            // First insert the just read pages into the cache
            spfsCacheReadExclusiveResponse* response = dynamic_cast<spfsCacheReadExclusiveResponse*>(msg);
            assert(0 != response);
            set<PagedCache::Key> readPages;
            getResponsePages(response, readPages);
            set<PagedCache::Key> writebackPages;
            updateCacheWithWritePages(readPages, writebackPages);
            resolvePendingReadPages(readPages);

            // Add the pending writebacks to the request's pending set
            registerPendingWritePages(write, writebackPages);
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

void PagedMiddlewareCacheMesi::getResponsePages(
    spfsCacheReadResponse* response,
    set<PagedCache::Key>& readPages) const
{
    assert(0 != response);
    size_t numPages = response->getPageIdsArraySize();
    Filename filename(response->getFilename());
    for (size_t i = 0; i < numPages; i++)
    {
        FilePageId pageId = response->getPageIds(i);
        PagedCache::Key key(filename, pageId);
        readPages.insert(key);
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
    set<PagedCache::Key>& requestPages,
    set<PagedCache::Key>& exclusivePages)
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
                exclusivePages.insert(*iter);
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

void PagedMiddlewareCacheMesi::updateCacheExclusivePages(
    const set<PagedCache::Key>& exclusivePages)
{
    set<PagedCache::Key>::const_iterator iter = exclusivePages.begin();
    set<PagedCache::Key>::const_iterator end = exclusivePages.end();
    while (iter != end)
    {
        try
        {
            PagedCache::Key key(iter->filename, iter->key);
            lruCache_->setState(key, MesiCacheType::MODIFIED);
            ++iter;
        } catch (NoSuchEntry& e)
        {
            assert(false);
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "Exclusive page should exist before updating state!!!\n";
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

void PagedMiddlewareCacheMesi::updateCacheWithWritePages(set<PagedCache::Key>& updatePages,
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
            lruCache_->insertAndRecall(*iter, iter->key, MesiCacheType::MODIFIED,
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

void PagedMiddlewareCacheMesi::clearDirectoryEntries(const set<PagedCache::Key>& entries)
{
    ClientCacheDirectory& directory = ClientCacheDirectory::instance();

    set<PagedCache::Key>::const_iterator iter = entries.begin();
    set<PagedCache::Key>::const_iterator last = entries.end();
    while (iter != last)
    {
        directory.removeClientCacheEntryByRank(getRank(),
                                               iter->filename,
                                               iter->key);
        iter++;
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
    assert(0 != pendingPages_);
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

void PagedMiddlewareCacheMesi::resolvePendingWritePage(const PagedCache::Key& resolvedPage)
{
    assert(0 != pendingPages_);
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        iter->second.writePages.erase(resolvedPage);
    }
}

void PagedMiddlewareCacheMesi::resolvePendingWritePages(const set<PagedCache::Key>& resolvedPages)
{
    assert(0 != pendingPages_);
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
    assert(0 != pendingPages_);
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
    assert(0 != pendingPages_);
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
    assert(0 != pendingPages_);
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
    assert(0 != pendingPages_);
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
