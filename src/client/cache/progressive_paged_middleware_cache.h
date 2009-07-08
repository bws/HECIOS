#ifndef PROGRESSIVE_PAGED_MIDDLEWARE_CACHE_H_
#define PROGRESSIVE_PAGED_MIDDLEWARE_CACHE_H_
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
#include "dirty_file_region_set.h"
#include "file_page.h"
#include "filename.h"
#include "middleware_cache.h"
#include "progressive_page_cache.h"
class spfsMPIFileRequest;
class spfsMPIFileCloseRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadAtResponse;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteAtResponse;

/**
 * A Progressive paged cache for a single node.  Multiple writers to the same
 * cache page will lead to incoherent page data as the old data will be
 * written to store.  IE.  this cache simply reads the page locally and
 * then updates until an evict or close forces the page out of cache.  No
 * attempts are made to prevent false sharing to unwritten page regions
 */
class ProgressivePagedMiddlewareCache : public MiddlewareCache
{
public:
    /** Data pending for an application request */
    struct PendingData
    {
        std::set<ProgressivePageCache::Key> readPages;
        std::set<spfsMPIFileRequest*> writeRequests;
    };

    /** A writeback entry */
    struct WritebackPage
    {
        Filename filename;

        FilePageId id;

        DirtyFileRegionSet regions;
    };

    /** Typedef of the type used to store file data internally */
    typedef ProgressivePageCache::Key Key;

    /** Typedef of the type used to store file data internally */
    typedef ProgressivePageCache::Page ProgressivePage;

    /** Typedef of the type used to store file data internally */
    typedef ProgressivePageCache FileDataPageCache;

    /** Typedef mapping a pending request to its pending requests */
    typedef std::map<spfsMPIFileRequest*, PendingData> RequestMap;

    /** Typedef mapping filenames to the number of current openers */
    typedef std::map<Filename, std::size_t> OpenFileMap;

    /** Constructor */
    ProgressivePagedMiddlewareCache();

protected:
    /** Perform module initialization */
    virtual void initialize();

    /** Perform module finalization */
    virtual void finish();

    /** @return the file data page cache for this middleware */
    virtual FileDataPageCache* createFileDataPageCache(size_t cacheSize);

    /** @return the map of pending reads indexed by request */
    virtual RequestMap* createPendingDataMap();

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
        std::set<Key>& outRequestPages) const;

    /** Read pages from the file system */
    void beginRead(const std::set<Key>& readPages,
                   spfsMPIFileRequest* parentRequest);

    /** Evict pages from the cache and write them to the file system */
    void beginWritebackEvictions(const std::vector<WritebackPage>& writebackPages,
                                 spfsMPIFileRequest* parentRequest);

    /** @return All the dirty cache entries for filename */
    std::vector<WritebackPage> lookupDirtyPagesInCache(const Filename& fileame) const;

    /** Remove request pages satisfied in the cache */
    void lookupPagesInCache(std::set<Key>& requestPages);

    /**
     * Remove all cache entries for the name flushFile
     */
    void flushCache(const Filename& flushFile);

    /**
     * Update the cache with pages marking the dirty status.  The resulting
     * writeback pages are returned in outWritebacks.
     */
    void updateCache(const std::set<Key>& requestPages,
                     bool isDirty,
                     std::vector<WritebackPage>& outWriteBacks);

    /**
     * Update the cache with pages marking the dirty status.  The resulting
     * writeback pages are returned in outWritebacks.
     */
    void updateCache(spfsMPIFileWriteAtRequest* write,
                     std::vector<WritebackPage>& outWriteBacks);

    /** Begin writing back the set of cache keys */
    void beginWritebacks(const std::vector<WritebackPage>& writeBacks,
                         spfsMPIFileRequest* req);

    /**
     * Send application responses for all of the pending requests in the
     * completed state.
     */
    void completeRequests();

    /** Register the pages pending to satisfy a request */
    void registerPendingReadPages(spfsMPIFileRequest* parentRequest,
                                  const std::set<Key>& readPages);

    /** Register the requests pending to satisfy a request */
    void registerPendingWriteRequests(spfsMPIFileRequest* parentRequest,
                                      const std::vector<spfsMPIFileWriteAtRequest*>& requests);

    /** Mark request as finished */
    void resolvePendingPages(const std::set<Key>& readPages);

    /** Mark request as finished */
    void resolvePendingRequest(spfsMPIFileRequest* finishedRequest);

    /** @return Removes and returns requests with no more pages remaining */
    std::vector<spfsMPIFileRequest*> popCompletedRequests();

    /** @return true if there are pending read or write pages for this file */
    bool hasPendingData(const Filename& filename) const;

    /** @return the set of page keys with pending pages removed */
    std::set<Key> trimPendingReadPages(const std::set<Key>& pageKeys);

    /** Data structure for holding the cached data */
    FileDataPageCache* lruCache_;

    /** Map of the number of opens for each file */
    OpenFileMap* openFileCounts_;

    /** Cache page capacity */
    std::size_t pageCapacity_;

    /** Cache progressive page size */
    FSSize pageSize_;

    /** Map of request to the total pending requests */
    RequestMap* pendingRequests_;

    /** Configure whether scalars are reported */
    bool recordScalars_;

    /** Track the maximum number of page regions */
    std::size_t maxPageRegions_;
};

#endif /* PROGRESSIVE_PAGED_MIDDLEWARE_CACHE_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
