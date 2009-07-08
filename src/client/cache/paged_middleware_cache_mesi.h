#ifndef PAGED_MIDDLEWARE_CACHE_MESI_H_
#define PAGED_MIDDLEWARE_CACHE_MESI_H_
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
#include "file_region_set.h"
#include "filename.h"
#include "lru_mesi_cache.h"
#include "page_access_mixin.h"
#include "paged_cache.h"
class spfsCacheReadRequest;
class spfsCacheReadResponse;
class spfsCacheReadExclusiveRequest;
class spfsCacheReadSharedRequest;
class spfsMPIFileRequest;
class spfsMPIFileCloseRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadAtResponse;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteAtResponse;

/**
 * A paged cache that uses the MESI invalidation protocol to achieve
 * cache coherence.
 */
class PagedMiddlewareCacheMesi : public PagedCache,
                                 private SinglePageAccessMixin
{
public:
    /** Typedef of the type used to store file data internally */
    typedef LRUMesiCache<PagedCache::Key, FilePageId> FileDataPageCache;

    /** Constructor */
    PagedMiddlewareCacheMesi();

protected:

    /** Typedef mapping a pending request to its pending cache pages */
    typedef std::map<spfsMPIFileRequest*, PagedCache::InProcessPages> RequestMap;

    /** Typedef mapping filenames to the number of current openers */
    typedef std::map<Filename, std::size_t> OpenFileMap;

    /** Message name for full page writeback requests */
    static const std::string PAGE_WRITEBACK_NAME;

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

    /** Cleanup memory associated with cache originated request */
    void cleanupRequest(cMessage* msg);

    /** Determine the set of pages for this I/O request */
    template<class spfsMPIFileIORequest> void getRequestCachePages(
        const spfsMPIFileIORequest* ioRequest,
        std::set<PagedCache::Key>& outRequestPages) const;

    /** Determine the set of pages for this I/O request */
    void getRequestCachePages(
        const spfsMPIFileWriteAtRequest* writeAt,
        std::vector<PagedCache::Key>& outFullPages,
        std::vector<PagedCache::Key>& outPartialPages) const;

    /** Determine the set of partial pages for this I/O request */
    template<class spfsMPIFileIORequest> void getRequestPartialCachePages(
        const spfsMPIFileIORequest* ioRequest,
        std::set<PagedCache::Key>& outRequestPages) const;

    /** @return The pages contained in the response */
    void getResponsePages(spfsCacheReadResponse* response,
                          std::set<PagedCache::Key>& readPages) const;

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
    void beginReadExclusive(const std::set<PagedCache::Key>& readPages,
                            spfsMPIFileRequest* parentRequest);

    /** Read pages from the file system */
    void beginReadShared(const std::set<PagedCache::Key>& readPages,
                         spfsMPIFileRequest* parentRequest);

    /** @return All the dirty cache entries for filename */
    std::set<PagedCache::Key> lookupModifiedPagesInCache(const Filename& fileame) const;

    /** Remove request pages satisfied in the cache */
    std::set<PagedCache::Key> lookupPagesInCache(std::set<PagedCache::Key>& requestPages);

    /** Remove request pages marked exclusive/modified in the cache */
    void trimExclusiveCachePages(std::set<PagedCache::Key>& requestPages,
                                 std::set<PagedCache::Key>& outExclusivePages);

    /** Tally in the additional read pages */
    void countPageArrivals(spfsCacheReadRequest* read, cMessage* response);

    /**
     * Remove all cache entries for the name flushFile
     */
    void flushCache(const Filename& flushFile);

    /** Mark exclusive pages modified */
    void updateCacheExclusivePages(const std::set<PagedCache::Key>& exclusivePages);

    /**
     * Update the cache with full pages that have been read.
     */
    void updateCacheWithReadPages(std::set<PagedCache::Key>& requestPages,
                                  std::set<PagedCache::Key>& outWriteBacks);

    /**
     * Update the cache with full pages that have been written.
     */
    void updateCacheWithWritePages(std::set<PagedCache::Key>& requestPages,
                                  std::set<PagedCache::Key>& outWriteBacks);

    /** Begin writing back the set of cache keys */
    void beginWritebacks(const std::set<PagedCache::Key>& writeBacks,
                         spfsMPIFileRequest* req);

    /**
     * Send application responses for all of the pending requests in the
     * completed state.
     */
    void completeRequests();

    /** Clear the exclusive/modified entries for these pages */
    void clearDirectoryEntries(const std::set<PagedCache::Key>& entries);

    /** Register all the pages pending to satisfy a request */
    void registerPendingPages(spfsMPIFileRequest* request,
                              const std::set<PagedCache::Key>& pendingReads,
                              const std::set<PagedCache::Key>& pendingWrites);

    /** Register all the pages pending to satisfy a request */
    void registerPendingWritePages(spfsMPIFileRequest* request,
                                   const std::set<PagedCache::Key>& pendingWrites);

    /** Mark page as read for requests */
    void resolvePendingReadPage(const PagedCache::Key& readPage);

    /** Mark pages as read for requests */
    void resolvePendingReadPages(const std::set<PagedCache::Key>& readPages);

    /** Mark page as written for requests */
    void resolvePendingWritePage(const PagedCache::Key& writePage);

    /** Mark pages as written for requests */
    void resolvePendingWritePages(const std::set<PagedCache::Key>& writePages);

    /** Decrement the partial count for the request */
    void resolvePendingPartialWrite(spfsMPIFileRequest* request);

    /** @return Removes and returns requests with no more pages remaining */
    std::vector<spfsMPIFileRequest*> popCompletedRequests();

    /** @return true if there are pending read or write pages for this file */
    bool hasPendingPages(const Filename& filename) const;

    /** Data structure for holding the cached data */
    FileDataPageCache* lruCache_;

    /** Map of request to the total pending pages */
    RequestMap* pendingPages_;

    /** Map of the number of opens for each file */
    OpenFileMap* openFileCounts_;

};

#endif /* PAGED_MIDDLEWARE_CACHE_MESI_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
