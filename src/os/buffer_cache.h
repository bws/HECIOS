#ifndef BLOCK_CACHE_H
#define BLOCK_CACHE_H
//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
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

/**
 * Abstract base class for OS Buffer Cache Managers
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
     *  @param msg (in) is the message to be processed.
     *
     *  @note This method is a hook function which is assumed to be
     *  provided by a derived class to (re)define the behaviour.
     */
    virtual bool isCached(LogicalBlockAddress address) = 0;

    /**
     * Add an entry to the cache
     *
     * @return the address value of the cache entry that is evicted
     *
     */
    virtual void addEntry(LogicalBlockAddress address) = 0;
    
private:

    int inGateId_;

    // Metric data collected at runtime
    double statNumRequests_;
    double statNumHits_;
    double statNumMisses_;
};

/**
 * Abstract base class for all buffer caches implementing a write-back
 * policy
 */
class WriteBackBufferCache : public BufferCache
{
protected:
    virtual void handleMessage(cMessage* msg);
    
    virtual long long getNextEviction(LogicalBlockAddress address) = 0;
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

    virtual void initializeCache() {};

    virtual bool isCached(LogicalBlockAddress address);

    virtual void addEntry(LogicalBlockAddress address) {};
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
     *  @copydoc AbstractCache::setup
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void initializeCache() {};

    /**
     *  @copydoc AbstractCache::checkCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual bool isCached(LogicalBlockAddress address);

    /**
     * Add an entry to the cache performing an eviction if neccesary
     */
    virtual void addEntry(LogicalBlockAddress address) {};
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
