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
#include "paged_middleware_cache_with_twin.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include "comm_man.h"
#include "file_builder.h"
#include "mpi_proto_m.h"
using namespace std;

/** Functor for finding all pages for a cached file */
class FilePageFilter : public MultiCache::FilterFunctor
{
public:
    /** Constructor */
    FilePageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file */
    virtual bool filter(const MultiCache::Key& key,
                        const MultiCache::Page& pageId,
                        bool isDirty) const
    {
        return (key.filename == filename_);
    }

private:
    Filename filename_;
};

/** Functor for finding dirty pages for a cached file */
class DirtyPageFilter : public MultiCache::FilterFunctor
{
public:
    /** Constructor */
    DirtyPageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file and is dirty */
    virtual bool filter(const MultiCache::Key& key,
                        const MultiCache::Page& value,
                        bool isDirty) const
    {
        return ((key.filename == filename_) && isDirty);
    }

private:
    Filename filename_;
};

// OMNet Registration Method
Define_Module(PagedMiddlewareCacheWithTwin);

const string PagedMiddlewareCacheWithTwin::PAGE_WRITEBACK_NAME("Page Writeback");

const string PagedMiddlewareCacheWithTwin::PARTIAL_PAGE_WRITEBACK_NAME = "PartialPage Writeback";

PagedMiddlewareCacheWithTwin::PagedMiddlewareCacheWithTwin()
    : lruCache_(0),
      pendingPages_(0),
      openFileCounts_(0)
{
}

void PagedMiddlewareCacheWithTwin::initialize()
{
    // Initialize parent
    PagedCache::initialize();

    // Initialize LRU management
    assert(0 != cacheCapacity());
    lruCache_ = createFileDataPageCache(cacheCapacity());

    // Initialize the pending request maps
    pendingPages_ = createPendingPageMap();
    pendingPartialPages_ = createPendingPartialPageMap();

    // Initialize the open file map
    openFileCounts_ = createOpenFileMap();
}

PagedMiddlewareCacheWithTwin::FileDataPageCache*
PagedMiddlewareCacheWithTwin::createFileDataPageCache(size_t cacheSize)
{
    return new FileDataPageCache(cacheSize);
}

PagedMiddlewareCacheWithTwin::RequestMap*
PagedMiddlewareCacheWithTwin::createPendingPageMap()
{
    return new RequestMap();
}

PagedMiddlewareCacheWithTwin::PartialRequestMap*
PagedMiddlewareCacheWithTwin::createPendingPartialPageMap()
{
    return new PartialRequestMap();
}

PagedMiddlewareCacheWithTwin::OpenFileMap*
PagedMiddlewareCacheWithTwin::createOpenFileMap()
{
    return new OpenFileMap();
}

void PagedMiddlewareCacheWithTwin::performFakeOpen(const Filename& openName)
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


void PagedMiddlewareCacheWithTwin::handleApplicationMessage(cMessage* msg)
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

void PagedMiddlewareCacheWithTwin::handleFileSystemMessage(cMessage* msg)
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

void PagedMiddlewareCacheWithTwin::processRequest(cMessage* request, cMessage* msg)
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

void PagedMiddlewareCacheWithTwin::processFileOpen(spfsMPIFileOpenRequest* open,
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

void PagedMiddlewareCacheWithTwin::processFileClose(spfsMPIFileCloseRequest* close, cMessage* msg)
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
            cerr << "Last close: Evictions are on" << endl;
            vector<CacheEntry> writeEntries = lookupDirtyPagesInCache(closeName);
            beginWritebackEvictions(writeEntries, close);
            flushCache(closeName);
            registerPendingWritePages(close, writeEntries);
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

        // If this is a partial page writeback, resolve it that way
        // Otherwise, resolve the pages normally
        if (PARTIAL_PAGE_WRITEBACK_NAME == flushReq->name())
        {
            resolvePendingPartialWrite(close);
        }
        else
        {
            // Resolve the pending pages
            set<PagedCache::Key> flushPages;
            getRequestCachePages(flushReq, flushPages);
            resolvePendingWritePages(flushPages);
        }
    }
}

void PagedMiddlewareCacheWithTwin::processFileRead(spfsMPIFileReadAtRequest* read, cMessage* msg)
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
        vector<CacheEntry> writebackPages;
        updateCacheWithReadPages(requestPages, writebackPages);

        // Update the pending requests
        resolvePendingReadPages(requestPages);

        beginWritebackEvictions(writebackPages, 0);

        // TODO: If the writebuffer is not infinite, the request will need
        // to pause while writebacks occur
        //addPendingWrites(read, writebackPages);
    }
}

void PagedMiddlewareCacheWithTwin::processFileWrite(spfsMPIFileWriteAtRequest* write, cMessage* msg)
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

                    // Figure out the memory copy delay for the read and write
                    double readBytes = partialPagesTrimmed.size() * pageSize();
                    double writeBytes = write->getCount() * write->getDataType()->getExtent();
                    double delay = (readBytes + writeBytes) * byteCopyTime();
                    cPar* delayPar = new cPar("Delay");
                    delayPar->setDoubleValue(delay);
                    write->addPar(delayPar);
                }
                else
                {
                    // Figure out the memory copy delay for the write only
                    double writeBytes = write->getCount() * write->getDataType()->getExtent();
                    double delay = (writeBytes) * byteCopyTime();
                    cPar* delayPar = new cPar("Delay");
                    delayPar->setDoubleValue(delay);
                    write->addPar(delayPar);

                    if (0 != allPartialPages.size())
                    {
                        cerr << __FILE__ << ":" << __LINE__ << ":"
                             << "WARNING: No partial reads need performing, but some are pending"
                             << " Perform one partial page read to waste time." << endl;
                        partialPagesTrimmed.insert(*(allPartialPages.begin()));
                        FSM_Goto(currentState, BEGIN_PARTIAL_PAGE_READ);
                    }
                    else
                    {
                        FSM_Goto(currentState, UPDATE_CACHE);
                    }
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
            // Transition to next state
            FSM_Goto(currentState, UPDATE_CACHE);
            break;
        }
        case FSM_Enter(UPDATE_CACHE):
        {
            // Get the full and partial pages this request spans
            vector<MultiCache::Page*> fullPages;
            vector<MultiCache::PartialPage*> partialPages;
            getRequestCachePages(write, fullPages, partialPages);

            // First insert the just read pages as original copies
            vector<CacheEntry> writebackPages;
            if (0 != dynamic_cast<spfsMPIFileReadAtResponse*>(msg))
            {
                spfsMPIFileReadAtRequest* read =
                    static_cast<spfsMPIFileReadAtRequest*>(msg->contextPointer());
                set<PagedCache::Key> readPages;
                getRequestCachePages(read, readPages);
                updateCacheWithReadPages(readPages, writebackPages);
                resolvePendingReadPages(readPages);
            }

            // Update the cache and acquire any necessary writebacks
            Filename filename = write->getFileDes()->getFilename();
            updateCacheWithWritePages(filename, fullPages, partialPages, writebackPages);

            // Update the pending requests with the pages fully written here
            resolvePendingReadPages(filename, fullPages);

            beginWritebackEvictions(writebackPages, 0);

            // TODO: If the writebuffer is not infinite, the request will need
            // to pause while writebacks occur
            //registerPendingWritePages(write, writebackPages);
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
void PagedMiddlewareCacheWithTwin::getRequestCachePages(
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

void PagedMiddlewareCacheWithTwin::getRequestCachePages(
    const spfsMPIFileWriteAtRequest* ioRequest,
    vector<MultiCache::Page*>& outFullPages,
    vector<MultiCache::PartialPage*>& outPartialPages) const
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
        MultiCache::Page* page = new MultiCache::Page();
        page->id = *first;
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
        MultiCache::PartialPage* page = new MultiCache::PartialPage();
        page->id = *first;
        page->regions = determinePartialPageRegions(page->id,
                                                    ioRequest->getOffset(),
                                                    ioSize,
                                                    fd->getFileView());
        outPartialPages.push_back(page);
        first++;
    }
}

template<class spfsMPIFileIORequest>
void PagedMiddlewareCacheWithTwin::getRequestPartialCachePages(
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


void PagedMiddlewareCacheWithTwin::beginWritebackEvictions(
    const vector<CacheEntry>& writebackPages,
    spfsMPIFileRequest* parentRequest)
{
    if (!writebackPages.empty())
    {
        // Sort the pages into two sets, full pages and partial pages
        map<Filename, set<FilePageId> > fullPagesMap;
        vector<CacheEntry> partialPages;
        for (size_t i = 0; i < writebackPages.size(); i++)
        {
            MultiCache::Page* page = writebackPages[i].second;
            assert(0 != page);
            if (0 == dynamic_cast<MultiCache::PartialPage*>(page))
            {
                Filename filename = writebackPages[i].first.filename;
                fullPagesMap[filename].insert(page->id);
            }
            else
            {
                partialPages.push_back(writebackPages[i]);
            }
        }

        // Create and send the requests for the partial pages
        for (size_t i = 0; i < partialPages.size(); i++)
        {
            Filename filename = partialPages[i].first.filename;
            MultiCache::PartialPage* page =
                static_cast<MultiCache::PartialPage*>(partialPages[i].second);
            FileRegionSet::iterator regIter = page->regions->begin();
            FileRegionSet::iterator regEnd = page->regions->end();
            while (regIter != regEnd)
            {
                // Create the file descriptor
                FileDescriptor* fd = FileBuilder::instance().getDescriptor(filename);
                spfsMPIFileWriteAtRequest* partialPageWriteback =
                    new spfsMPIFileWriteAtRequest(PARTIAL_PAGE_WRITEBACK_NAME.c_str(),
                                                  SPFS_MPI_FILE_WRITE_AT_REQUEST);
                partialPageWriteback->setContextPointer(parentRequest);
                partialPageWriteback->setFileDes(fd);
                partialPageWriteback->setDataType(new ByteDataType());
                partialPageWriteback->setOffset(regIter->offset);
                partialPageWriteback->setCount(regIter->extent);
                send(partialPageWriteback, fsOutGateId());
                regIter++;
            }
        }

        // Create and send the requests for the full pages
        map<Filename, set<FilePageId> >::iterator iter = fullPagesMap.begin();
        map<Filename, set<FilePageId> >::iterator last = fullPagesMap.end();
        while (iter != last)
        {
            spfsMPIFileWriteAtRequest* writebackRequest =
                createPageWriteRequest(iter->first,
                                           iter->second,
                                           parentRequest);
            send(writebackRequest, fsOutGateId());
            iter++;
        }

        // TODO: Should probably now loop thru all of the cache entries and free
        // the related memory
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "The writeback set was empty." << endl;
    }
}

void PagedMiddlewareCacheWithTwin::beginRead(
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

vector<PagedMiddlewareCacheWithTwin::CacheEntry>
PagedMiddlewareCacheWithTwin::lookupDirtyPagesInCache(const Filename& filename) const
{
    vector<CacheEntry> dirtyEntries;
    DirtyPageFilter filter(filename);
    vector<MultiCache::Page*> dirtyPages = lruCache_->getFilteredEntries(filter);
    for (size_t i = 0; i < dirtyPages.size(); i++)
    {
        MultiCache::Key key(filename, dirtyPages[i]->id);
        dirtyEntries.push_back(make_pair(key, dirtyPages[i]));
    }
    return dirtyEntries;
}

void PagedMiddlewareCacheWithTwin::lookupPagesInCache(set<PagedCache::Key>& requestPages)
{
    set<PagedCache::Key>::iterator iter = requestPages.begin();
    set<PagedCache::Key>::iterator end = requestPages.end();
    while (iter != end)
    {
        try
        {
            MultiCache::Key key(iter->filename, iter->key);
            lruCache_->lookup(key);
            requestPages.erase(iter++);
        } catch (NoSuchEntry& e)
        {
            // Lookup failed, increment to next entry
            iter++;
        }
    }
}

void PagedMiddlewareCacheWithTwin::updateCacheWithReadPages(set<PagedCache::Key>& updatePages,
                                                            vector<CacheEntry>& outWriteBacks)
{
    // Update the cache and accumulate any writebacks required
    set<PagedCache::Key>::const_iterator iter;
    for (iter = updatePages.begin(); iter != updatePages.end(); iter++)
    {
        try {
            // Create an entry for the clean page even if the dirty page
            // already exists
            MultiCache::Key key(iter->filename, iter->key);
            MultiCache::Page* page = new MultiCache::Page();
            page->id = iter->key;

            MultiCache::Key evictedKey(Filename("/"), 0);
            MultiCache::Page* evictedPage = 0;
            bool isEvictedDirty = false;
            lruCache_->insertFullPageAndRecall(key, page, false,
                                               evictedKey,
                                               evictedPage,
                                               isEvictedDirty);

            // Only add writeback if the eviction is dirty
            if (isEvictedDirty)
            {
                assert(0 != evictedPage);
                outWriteBacks.push_back(make_pair(evictedKey, evictedPage));
            }
        } catch (const NoSuchEntry& e)
        {
            // No eviction was necessary
        }
    }
}

void PagedMiddlewareCacheWithTwin::updateCacheWithWritePages(
    const Filename& filename,
    const vector<MultiCache::Page*>& fullPages,
    const vector<MultiCache::PartialPage*>& partialPages,
    vector<CacheEntry>& outWritebacks)
{
    // Update the cache with full pages and accumulate any writebacks
    for (size_t i = 0; i < fullPages.size(); i++)
    {
        MultiCache::Key key(filename, fullPages[i]->id);
        MultiCache::Key evictedKey(Filename("/"), 0);
        MultiCache::Page* evictedPage = 0;
        bool isEvictedDirty = false;
        try {
            lruCache_->insertFullPageAndRecall(key, fullPages[i], true,
                                               evictedKey,
                                               evictedPage,
                                               isEvictedDirty);

            // Add the writeback entry
            if (isEvictedDirty)
            {
                assert(0 != evictedPage);
                outWritebacks.push_back(make_pair(evictedKey, evictedPage));
            }
        } catch(NoSuchEntry& e)
        {
            // No eviction occurred
        }
    }

    // Update the cache with partial pages
    for (size_t i = 0; i < partialPages.size(); i++)
    {
        MultiCache::Key key(filename, partialPages[i]->id);
        MultiCache::Key evictedKey(Filename("/"), 0);
        MultiCache::Page* evictedPage = 0;
        bool isEvictedDirty = false;
        try {
            // Then we insert the dirty partials
            lruCache_->insertDirtyPartialPageAndRecall(key, partialPages[i],
                                                       evictedKey,
                                                       evictedPage,
                                                       isEvictedDirty);

            // Add the writeback entry
            if (isEvictedDirty)
            {
                assert(0 != evictedPage);
                outWritebacks.push_back(make_pair(evictedKey, evictedPage));
            }
        } catch(NoSuchEntry& e)
        {
            // No eviction occurred
        }
    }
}

void PagedMiddlewareCacheWithTwin::flushCache(const Filename& flushName)
{
    if (0 == (*openFileCounts_)[flushName])
    {
        FilePageFilter filter(flushName);
        vector<MultiCache::Page*> flushPages = lruCache_->getFilteredEntries(filter);
        vector<MultiCache::Page*>::const_iterator begin = flushPages.begin();
        vector<MultiCache::Page*>::const_iterator end = flushPages.end();
        while (begin != end)
        {
            try
            {
                MultiCache::Key removeKey(flushName, (*begin)->id);
                lruCache_->remove(removeKey);
                begin++;
            }
            catch(NoSuchEntry& nse)
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "No such entry while flushing a file from the cache: "
                     << flushName << " " << (*begin)->id << endl;
                assert(0);
            }
        }
    }
}

void PagedMiddlewareCacheWithTwin::completeRequests()
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

void PagedMiddlewareCacheWithTwin::registerPendingPages(
    spfsMPIFileRequest* fileRequest,
    const set<PagedCache::Key>& readPages,
    const set<PagedCache::Key>& writePages)
{
    PagedCache::InProcessPages& inProcess = (*pendingPages_)[fileRequest];
    inProcess.readPages.insert(readPages.begin(), readPages.end());
    inProcess.writePages.insert(writePages.begin(), writePages.end());
}

void PagedMiddlewareCacheWithTwin::registerPendingWritePages(spfsMPIFileRequest* request,
                                                             const vector<CacheEntry>& pendingWrites)
{
    PagedCache::InProcessPages& inProcess = (*pendingPages_)[request];
    size_t partialCount = 0;
    for (size_t i = 0; i < pendingWrites.size(); i++)
    {
        MultiCache::Page* page = pendingWrites[i].second;
        if (0 == dynamic_cast<MultiCache::PartialPage*>(page))
        {
            // Add the full pages to the in process list
            PagedCache::Key writePage(pendingWrites[i].first.filename, page->id);
            inProcess.writePages.insert(writePage);
        }
        else
        {
            // Simply count the the partials
            MultiCache::PartialPage* partial = dynamic_cast<MultiCache::PartialPage*>(page);
            assert(0 != partial);
            partialCount += partial->regions->size();
        }
    }
    if (0 != partialCount)
    {
        (*pendingPartialPages_)[request] = partialCount;
    }
}

void PagedMiddlewareCacheWithTwin::resolvePendingReadPage(const PagedCache::Key& resolvedPage)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        iter->second.readPages.erase(resolvedPage);
    }
}

void PagedMiddlewareCacheWithTwin::resolvePendingReadPages(const set<PagedCache::Key>& resolvedPages)
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

void PagedMiddlewareCacheWithTwin::resolvePendingReadPages(const Filename& filename,
                                                           const vector<MultiCache::Page*>& readPages)
{
    set<PagedCache::Key> readKeys;
    for (size_t i = 0; i < readPages.size(); i++)
    {
        PagedCache::Key key(filename, readPages[i]->id);
        readKeys.insert(key);
    }
    resolvePendingReadPages(readKeys);
}

void PagedMiddlewareCacheWithTwin::resolvePendingWritePage(const PagedCache::Key& resolvedPage)
{
    RequestMap::iterator iter;
    for (iter = pendingPages_->begin(); pendingPages_->end() != iter; ++iter)
    {
        iter->second.writePages.erase(resolvedPage);
    }
}

void PagedMiddlewareCacheWithTwin::resolvePendingWritePages(const set<PagedCache::Key>& resolvedPages)
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

void PagedMiddlewareCacheWithTwin::resolvePendingPartialWrite(spfsMPIFileRequest* request)
{
    assert(0 != pendingPartialPages_->count(request));
    --((*pendingPartialPages_)[request]);
}


vector<spfsMPIFileRequest*> PagedMiddlewareCacheWithTwin::popCompletedRequests()
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
                // no other reads or writes for this file are ongoing and the
                // partial count is zero
                spfsMPIFileRequest* request = currentEle->first;
                Filename closeName = request->getFileDes()->getFilename();
                if (!hasPendingPages(closeName) &&
                    0 == (*pendingPartialPages_)[request])
                {
                    completeRequests.push_back(currentEle->first);
                    pendingPages_->erase(currentEle);
                    pendingPartialPages_->erase(request);
                }
            }
        }
    }
    return completeRequests;
}

set<PagedCache::Key> PagedMiddlewareCacheWithTwin::trimPendingReadPages(const set<PagedCache::Key>& pageIds)
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

set<PagedCache::Key> PagedMiddlewareCacheWithTwin::trimPendingWritePages(const set<PagedCache::Key>& pages)
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

bool PagedMiddlewareCacheWithTwin::hasPendingPages(const Filename& filename) const
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
