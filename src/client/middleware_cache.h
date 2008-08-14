#ifndef MIDDLEWARE_CACHE_H_
#define MIDDLEWARE_CACHE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
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
#include <vector>
#include <omnetpp.h>
#include "basic_types.h"
#include "file_page.h"
class FileView;
class spfsMPIFileReadAtRequest;
class spfsMPIFileWriteAtRequest;

/**
 * An abstract model of a middleware file system data cache.
 */
class MiddlewareCache : public cSimpleModule
{
public:
    /** Constructor */
    MiddlewareCache();

    /** Abstract destructor */
    ~MiddlewareCache() = 0;
    
    /** @return the appIn gate id */
    int appInGateId() const { return appInGateId_; };
    
    /** @return the appOut gate id */
    int appOutGateId() const { return appOutGateId_; };
        
    /** @return the fsIn gate id */
    int fsInGateId() const { return fsInGateId_; };

    /** @return the fsOut gate id */
    int fsOutGateId() const { return fsOutGateId_; };


protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    /** Interface for handling messages from the application */
    virtual void handleApplicationMessage(cMessage* msg) = 0;
    
    /** Interface for handling messages from the file system */
    virtual void handleFileSystemMessage(cMessage* msg) = 0;

    /** Gate ids */
    int appInGateId_;
    int appOutGateId_;
    int fsInGateId_;
    int fsOutGateId_;
    
    /** The number of cache hits */
    std::size_t numCacheHits_;
    
    /** The number of cache misses */
    std::size_t numCacheMisses_;
    
    /** The number of cache evictions */
    std::size_t numCacheEvicts_;
};

/** An abstract page aligned cache */
class PagedCache : public MiddlewareCache
{
public:
    /** Constructor */
    PagedCache();
  
    /** Abstract destructor */
    virtual ~PagedCache() = 0;
    
    /** @return the number of pages the cache can contain */
    size_t cacheCapacity() const { return pageCapacity_; };

    /** @return the cache's page size */
    size_t pageSize() const { return pageSize_; };
    
    /** @return the first offset for the page */
    FSOffset pageBeginOffset(const FilePageId& pageId) const;
    
    /** @return Array of pages ids spanning the supplied file regions */
    std::vector<FilePageId> determineRequestPages(const FSOffset& offset,
                                                  const FSSize& size,
                                                  const FileView& view);
    
protected:
    /** Module initialization */
    void initialize();

    /** @return a request to read the desired pages */
    spfsMPIFileReadAtRequest* createPageReadRequest(
        const std::vector<FilePageId>& pageIds) const;

    /** @return a request to write the desired pages */
    spfsMPIFileWriteAtRequest* createPageWriteRequest(
        const std::vector<FilePageId>& pageIds) const;

private:
    /** @return Array of page ids spanning the supplied file regions */
    std::vector<FilePageId> regionsToPageIds(
        const std::vector<FileRegion>& fileRegions);
    
    /** @return Array of pages spanning the supplied file regions */
    std::vector<FilePage> regionsToPages(
        const std::vector<FileRegion>& fileRegions);
    
    /** Page size attribute */
    std::size_t pageSize_;
    
    /** Page capacity attribute */
    std::size_t pageCapacity_;
};

/** Model of a cache that does simple pass through semantics */
class NoMiddlewareCache : public MiddlewareCache
{
public:
    /** Constructor */
    NoMiddlewareCache();
    
private:
    /** Forward application messages to file system */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Forward application messages to file system */
    virtual void handleFileSystemMessage(cMessage* msg);
};

/** A Direct paged cache for a single node */
class DirectPagedMiddlewareCache : public PagedCache
{
public:
    /** Constructor */
    DirectPagedMiddlewareCache();

private:
    /** Handle messages received from the application */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Handle messages received from the file system */
    virtual void handleFileSystemMessage(cMessage* msg);

    /** @return true if all of the pages are resident
     *  
     * Side effect: Retrieves non-resident pages
     */ 
    bool lookupData(const std::vector<FilePageId> requestPageIds);
};

/** A fully associative paged cache for a single node */
class FullyPagedMiddlewareCache : public PagedCache
{
public:
    /** Constructor */
    FullyPagedMiddlewareCache();

private:
    virtual void handleApplicationMessage(cMessage* msg);
    
    virtual void handleFileSystemMessage(cMessage* msg);

    /** @return true if all of the pages are resident
     *  
     * Side effect: Retrieves non-resident pages
     */ 
    bool lookupData(const std::vector<FilePage> requestPages);
};

/** A cooperative direct paged cache for a single node */
class CooperativeDirectMiddlewareCache : public PagedCache
{
public:
    CooperativeDirectMiddlewareCache();

private:
    virtual void handleApplicationMessage(cMessage* msg);
    
    virtual void handleFileSystemMessage(cMessage* msg);
};

#endif /*MIDDLEWARE_CACHE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
