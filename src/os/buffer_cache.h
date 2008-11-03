#ifndef BLOCK_CACHE_H
#define BLOCK_CACHE_H
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

#include <omnetpp.h>
#include "basic_types.h"
#include "lru_cache.h"

/**
 * Abstract base class for OS Buffer Cache Managers
 *
 * Provides an interface to a write back cache for its message handler
 */
class BufferCache : public cSimpleModule
{
public:
    /**
     *  This is the constructor for this simulation module.
     */
    BufferCache();

protected:
    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void finish();

    /**
     *  Implements a simple *write-though* buffer cache
     *
     *  @param msg (in) is the message to be processed.
     */
    virtual void handleMessage(cMessage* msg);

    /**
     *  This method is invoked at the beginning of the simulation
     *  run by the @ref AbstractCache::initialize method to allow for
     *  Cache algorithm specific initialization actions.
     *
     *  @param msg (in) is the message to be processed.
     *
     *  @note This method is a hook function which is assumed to be
     *  provided by a derived class to (re)define the behaviour.
     */
    virtual void initializeCache() = 0;

    /**
     *  This method is invoked at the end of the simulation
     *  run by the @ref BufferCache::finish method to allow for
     *  Cache algorithm specific finalization actions.
     *
     *  @param msg (in) is the message to be processed.
     *
     *  @note This method is a hook function which is assumed to be
     *  provided by a derived class to (re)define the behavior.
     */
    virtual void finalizeCache() = 0;

    /** Register a buffer cache hit */
    void registerHit();

    /** Register a buffer cache miss */
    void registerMiss();

    /** Register a write through */
    void registerWriteThrough();

private:
    /** Handle caching for incoming block requests from the file system */
    virtual void handleBlockRequest(cMessage* msg) = 0;

    /** Handle caching for incoming block device responses */
    virtual void handleBlockResponse(cMessage* msg) = 0;

    /** in gate id */
    int inGateId_;

    // Metric data collected at runtime
    double statNumRequests_;
    double statNumHits_;
    double statNumMisses_;
    double statNumWriteThroughs_;
};

/**
 * No cache: all requests are passed through to the Disk Subsystem
 */
class NoBufferCache : public BufferCache
{
  public:
    /**
     *  This is the constructor for this simulation module.
     */
    NoBufferCache();

protected:
    /** No-op */
    virtual void initializeCache();

    /** No-op */
    virtual void finalizeCache();

private:
    /** Handle caching for incoming block requests from the file system */
    virtual void handleBlockRequest(cMessage* msg);

    /** Handle caching for incoming block device responses */
    virtual void handleBlockResponse(cMessage* msg);
};

/**
 * Least Recently Used Cache Manager
 */
class LRUBufferCache : public BufferCache
{
public:
    /**
     *  This is the constructor for this simulation module.
     */
    LRUBufferCache();

protected:
    /**
     *
     */
    struct Entry
    {
        LogicalBlockAddress lba;
        bool isDirty;
    };

    /** Initialize this cache */
    virtual void initializeCache();

    /** Finalize this cache */
    virtual void finalizeCache();

    /**
     */
    virtual bool isCached(LogicalBlockAddress address);

    /** @return true if the cache is full */
    virtual bool isFull();

    /** @return the least recently used entry */
    virtual Entry getNextEviction();

private:
    /** Handle caching for incoming block requests from the file system */
    virtual void handleBlockRequest(cMessage* msg);

    /** Handle caching for incoming block device responses */
    virtual void handleBlockResponse(cMessage* msg);

    /**
     * Evict a cache entry if the cache is full and write the block to disk
     * if it is marked dirty
     */
    void evictCacheEntry(LogicalBlockAddress lba);

    /** Dirty percent threshold parameter */
    double dirtyThreshold_;

    /** Cache helper class */
    LRUCache<LogicalBlockAddress, char>* cache_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
