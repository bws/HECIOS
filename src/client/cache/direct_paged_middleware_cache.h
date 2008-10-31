#ifndef DIRECT_PAGED_MIDDLEWARE_CACHE_H_
#define DIRECT_PAGED_MIDDLEWARE_CACHE_H_
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
#include "filename.h"
#include "lru_cache.h"
#include "paged_cache.h"
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadAtResponse;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteAtRequest;

/** A Direct paged cache for a single node */
class DirectPagedMiddlewareCache : public PagedCache
{
public:
    /** Constructor */
    DirectPagedMiddlewareCache();

protected:
    /** Typedef of the type used to store file data internally */
    typedef LRUCache<PagedCache::Key, FilePageId> FileDataPageCache;

    /** Typedef mapping a pending request to its pending pages */
    typedef std::map<spfsMPIFileRequest*, std::set<FilePageId> > RequestMap;

    /** Typedef mapping filenames to the number of current openers */
    typedef std::map<Filename, std::size_t> OpenFileMap;

    /** Perform module initialization */
    virtual void initialize();

    /** @return the file data page cache for this middleware */
    virtual FileDataPageCache* createFileDataPageCache(size_t cacheSize);

    /** @return the map of pending requests for this middleware */
    virtual RequestMap* createPendingRequestMap();

    /**
     * @return the map of the number of times each file has been opened for
     *   this middleware
     */
    virtual OpenFileMap* createOpenFileMap();

    /** Increment the file open count */
    void processFileOpen(const Filename& openName);

    /**
     * Decrement the file open count and flush dirty file data to disk if
     * this is the last close for this file
     */
    void processFileClose(const Filename& closeName);

private:
    /** Handle messages received from the application */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Handle messages received from the file system */
    virtual void handleFileSystemMessage(cMessage* msg);

    /**
     * @return Pages that must be requested for this read.  Note that pages
     * in cache or currently in-flight for this request are not returned.
     *
     * @side Registers the unsatisfied pages for the request
     */
    std::set<FilePageId> resolveRequest(spfsMPIFileReadAtRequest* readRequest);

    /**
     * @return Pages that must be requested for this write.  Only partial pages
     * not in the cache or currently in-flight
     *
     * @side Registers the unsatisfied pages for the request
     */
    std::set<FilePageId> resolveRequest(spfsMPIFileWriteAtRequest* writeRequest);

    /**
     * Update the cache with pages that are fully written by this request
     */
    void updateCache(spfsMPIFileWriteAtRequest* writeRequest);

    /**
     * Update the cache with pages that have been read from the file system
     */
    void updateCache(spfsMPIFileReadAtResponse* readResponse);

    /**
     * Update the cache with pages marking the dirty status.  The resulting
     * writeback pages are returned in outWritebacks.
     */
    void updateCache(const Filename& filename,
                     const std::set<FilePageId>& updatePages,
                     bool updatesDirty,
                     std::set<PagedCache::Key>& outWriteBacks);

    /**
     * Send application responses for all of the pending requests in the
     * completed state.
     */
    void completeRequests();

    /**
     * Complete the writebacks contained in the set of page keys
     */
    void completeWriteBacks(const std::set<PagedCache::Key>& writeBacks,
                            spfsMPIFileRequest* req);

    /** Register all the pages pending to satisfy a request */
    void registerPendingRequest(spfsMPIFileRequest* request,
                                const std::set<FilePageId>& pendingPages);

    /** Mark pages as no longer pending for requests */
    void updatePendingRequests(const std::set<FilePageId>& pageIds);

    /** @return Removes and returns requests with no more pages remaining */
    std::vector<spfsMPIFileRequest*> popCompletedRequests();

    /** Remove pages already registered and requested as pending from the set */
    std::set<FilePageId> removeRequestedPages(std::set<FilePageId>& pageIds) const;

    /** Data structure for holding the cached data */
    LRUCache<PagedCache::Key, FilePageId>* lruCache_;

    /** Map of request to the total pending pages */
    RequestMap* pendingRequests_;

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
