#ifndef PAGED_MIDDLEWARE_CACHE_WITH_TWIN_NO_BLOCK_INDEXED_H_
#define PAGED_MIDDLEWARE_CACHE_WITH_TWIN_NO_BLOCK_INDEXED_H_
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
#include <cstddef>
#include <map>
#include <set>
#include <omnetpp.h>
#include "basic_types.h"
#include "file_page.h"
#include "file_region_set.h"
#include "filename.h"
#include "multi_cache.h"
#include "paged_cache.h"
class spfsMPIFileCloseRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadAtResponse;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteAtResponse;

/** A paged cache that creates a twin to support partial page writes */
class PagedMiddlewareCacheWithTwinNoBlockIndexed : public PagedCache
{
public:
    /** Constructor */
    PagedMiddlewareCacheWithTwinNoBlockIndexed();

protected:
    /** Typedef of the type used to store file data internally */
    typedef MultiCache FileDataPageCache;

    /** Typedef of the data stored for a cache line */
    typedef std::pair<MultiCache::Key, MultiCache::Page*> CacheEntry;

    /** Typedef mapping a pending request to its pending cache pages */
    typedef std::map<spfsMPIFileRequest*, PagedCache::InProcessPages> RequestMap;

    /** Typedef mapping a pending request to its number of partial cache pages */
    typedef std::map<spfsMPIFileRequest*, int> PartialRequestMap;

    /** Typedef mapping filenames to the number of current openers */
    typedef std::map<Filename, std::size_t> OpenFileMap;

    /** Message name for full page writeback requests */
    static const std::string PAGE_READ_NAME;

    /** Message name for full page writeback requests */
    static const std::string PAGE_WRITEBACK_NAME;

    /** Message name for partial page writeback requests */
    static const std::string PARTIAL_PAGE_WRITEBACK_NAME;

    /** Perform module initialization */
    virtual void initialize();

    /** @return the file data page cache for this middleware */
    virtual FileDataPageCache* createFileDataPageCache(size_t cacheSize);

    /** @return the map of pending reads indexed by request */
    virtual RequestMap* createPendingPageMap();

    /** @return the map of pending reads indexed by request */
    virtual PartialRequestMap* createPendingPartialPageMap();

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

    /** Determine the set of pages for this I/O request */
    void getRequestCachePages(
        const spfsMPIFileWriteAtRequest* writeAt,
        std::vector<MultiCache::Page>& outFullPages,
        std::vector<MultiCache::PartialPage>& outPartialPages) const;

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
    void beginWritebackEvictions(const std::vector<CacheEntry>& writebackPages,
                                 spfsMPIFileRequest* parentRequest);

    /** Read pages from the file system */
    void beginRead(const std::set<PagedCache::Key>& readPages,
                   spfsMPIFileRequest* parentRequest);

    /** @return All the dirty cache entries for filename */
    std::vector<CacheEntry> lookupDirtyPagesInCache(const Filename& fileame) const;

    /** Remove request pages satisfied in the cache */
    std::set<PagedCache::Key> lookupPagesInCache(std::set<PagedCache::Key>& requestPages);

    /**
     * Remove all cache entries for the name flushFile
     */
    void flushCache(const Filename& flushFile);

    /**
     * Update the cache with full pages that have been read.
     */
    void updateCacheWithReadPages(std::set<PagedCache::Key>& requestPages,
                                  std::vector<CacheEntry>& outWriteBacks);

    /**
     * Update the cache with pages that have been written.
     */
    void updateCacheWithWritePages(const Filename& filename,
                                   const std::vector<MultiCache::Page>& fullPages,
                                   const std::vector<MultiCache::PartialPage>& partialPages,
                                   std::vector<CacheEntry>& outWriteBacks);

    /**
     * Update the cache with pages that have been partially updated
     */
    void updateCacheWithReadPageUpdates(spfsMPIFileWriteAtRequest* writeAt,
                                        std::set<PagedCache::Key>& requestPages,
                                        std::vector<CacheEntry>& outWriteBacks);

    /** Begin writing back the set of cache keys */
    void beginWritebacks(const std::vector<CacheEntry>& writeBacks,
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

    /** Register all the pages pending to satisfy a request */
    void registerPendingWritePages(spfsMPIFileRequest* request,
                                   const std::vector<CacheEntry>& pendingWrites);

    /** Mark page as read for requests */
    void resolvePendingReadPage(const PagedCache::Key& readPage);

    /** Mark pages as read for requests */
    void resolvePendingReadPages(const std::set<PagedCache::Key>& readPages);

    /** Mark pages as read for requests */
    void resolvePendingReadPages(const Filename& filename,
                                 const std::vector<MultiCache::Page>& readPages);

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

    /** Data structure for tracking pending reads */
    std::map<spfsMPIFileRequest*, int> pendingReadCount_;

    /** Data structure for holding the cached data */
    MultiCache* lruCache_;

    /** Map of request to the total pending pages */
    RequestMap* pendingPages_;

    /** Map of request to the total pending pages */
    PartialRequestMap* pendingPartialPages_;

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
