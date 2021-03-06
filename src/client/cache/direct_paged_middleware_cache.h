#ifndef DIRECT_PAGED_MIDDLEWARE_CACHE_H_
#define DIRECT_PAGED_MIDDLEWARE_CACHE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <map>
#include <set>
#include <omnetpp.h>
#include "basic_types.h"
#include "file_page.h"
#include "filename.h"
#include "lru_cache.h"
#include "paged_cache.h"
class spfsMPIFileCloseRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadAtResponse;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteAtResponse;

/**
 * A Direct paged cache for a single node.  Multiple writers to the same
 * cache page will lead to incoherent page data as the old data will be
 * written to store.  IE.  this cache simply reads the page locally and
 * then updates until an evict or close forces the page out of cache.  No
 * attempts are made to prevent false sharing to unwritten page regions
 */
class DirectPagedMiddlewareCache : public PagedCache
{
public:
    /** Constructor */
    DirectPagedMiddlewareCache();

protected:
    /** Typedef of the type used to store file data internally */
    typedef LRUCache<PagedCache::Key, FilePageId> FileDataPageCache;

    /** Typedef mapping a pending request to its pending cache pages */
    typedef std::map<spfsMPIFileRequest*, PagedCache::InProcessPages> RequestMap;

    /** Typedef mapping filenames to the number of current openers */
    typedef std::map<Filename, std::size_t> OpenFileMap;

    /** Perform module initialization */
    virtual void initialize();

    /** @return the file data page cache for this middleware */
    virtual FileDataPageCache* createFileDataPageCache(size_t cacheSize);

    /** @return the map of pending reads indexed by request */
    virtual RequestMap* createPendingPageMap();

    /**
     * @return the map of the number of times each file has been opened for
     *   this middleware
     */
    virtual OpenFileMap* createOpenFileMap();

    /**
     * Allow processes that don't send an actual MPI_FILE_OPEN call to the
     * cache to still increment the file open count.
     */
    virtual void performFakeOpen(const Filename& openName);

private:
    /** Handle messages received from the application */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Handle messages received from the file system */
    virtual void handleFileSystemMessage(cMessage* msg);

    void processRequest(cMessage* request, cMessage* msg);

    void processFileOpen(spfsMPIFileOpenRequest* open, cMessage* msg);

    void processFileClose(spfsMPIFileCloseRequest* close, cMessage* msg);

    void processFileRead(spfsMPIFileReadAtRequest* read, cMessage* msg);

    void processFileWrite(spfsMPIFileWriteAtRequest* write, cMessage* msg);

    /** Determine the set of pages for this I/O request */
    template<class spfsMPIFileIORequest> void getRequestCachePages(
        const spfsMPIFileIORequest* ioRequest,
        std::set<PagedCache::Key>& outRequestPages) const;

    /** Determine the set of partial pages for this I/O request */
    template<class spfsMPIFileIORequest> void getRequestPartialCachePages(
        const spfsMPIFileIORequest* ioRequest,
        std::set<PagedCache::Key>& outRequestPages) const;

    /** @return pages from readPages not already scheduled for reading */
    std::set<PagedCache::Key> trimPendingReadPages(const std::set<PagedCache::Key>& readPages);

    /** @return pages from writePages not already scheduled for writing */
    std::set<PagedCache::Key> trimPendingWritePages(const std::set<PagedCache::Key>& writePages);

    void findEvictions(const std::set<PagedCache::Key> newPages,
                       std::set<PagedCache::Key>& outWritebacks) const;

    /** Evict pages from the cache and write them to the file system */
    void beginWritebackEvictions(const std::set<PagedCache::Key>& writebackPages,
                                 spfsMPIFileRequest* parentRequest);

    /** Read pages from the file system */
    void beginRead(const std::set<PagedCache::Key>& readPages,
                   spfsMPIFileRequest* parentRequest);

    /** @return All the dirty cache entries for filename */
    std::set<PagedCache::Key> lookupDirtyPagesInCache(const Filename& fileame) const;

    /** Remove request pages satisfied in the cache */
    void lookupPagesInCache(std::set<PagedCache::Key>& requestPages);

    /**
     * Remove all cache entries for the name flushFile
     */
    void flushCache(const Filename& flushFile);

    /**
     * Update the cache with pages marking the dirty status.  The resulting
     * writeback pages are returned in outWritebacks.
     */
    void updateCache(std::set<PagedCache::Key>& requestPages,
                     bool updatesDirty,
                     std::set<PagedCache::Key>& outWriteBacks);

    /** Begin writing back the set of cache keys */
    void beginWritebacks(const std::set<PagedCache::Key>& writeBacks,
                            spfsMPIFileRequest* req);

    /**
     * Send application responses for all of the pending requests in the
     * completed state.
     */
    void completeRequests();

    /** Register all the pages pending to satisfy a request */
    void registerPendingPages(spfsMPIFileRequest* request,
                                const std::set<PagedCache::Key>& pendingReads,
                                const std::set<PagedCache::Key>& pendingWrites);

    /** Mark page as read for requests */
    void resolvePendingReadPage(const PagedCache::Key& readPage);

    /** Mark pages as read for requests */
    void resolvePendingReadPages(const std::set<PagedCache::Key>& readPages);

    /** Mark page as written for requests */
    void resolvePendingWritePage(const PagedCache::Key& writePage);

    /** Mark pages as written for requests */
    void resolvePendingWritePages(const std::set<PagedCache::Key>& writePages);

    /** @return Removes and returns requests with no more pages remaining */
    std::vector<spfsMPIFileRequest*> popCompletedRequests();

    /** @return true if there are pending read or write pages for this file */
    bool hasPendingPages(const Filename& filename) const;

    /** Data structure for holding the cached data */
    LRUCache<PagedCache::Key, FilePageId>* lruCache_;

    /** Map of request to the total pending pages */
    RequestMap* pendingPages_;

    /** Map of the number of opens for each file */
    OpenFileMap* openFileCounts_;
};

#endif /* DIRECT_PAGED_MIDDLEWARE_CACHE_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
