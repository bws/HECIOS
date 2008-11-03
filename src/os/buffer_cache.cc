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
#include <stdexcept>
#include "buffer_cache.h"
#include "os_proto_m.h"
using namespace std;

//=============================================================================
//
// BufferCache implementation (abstract class)
//
//=============================================================================

BufferCache::BufferCache()
{
}

void BufferCache::initialize()
{
    // Initialize
    statNumRequests_ = 0;
    statNumHits_ = 0;
    statNumMisses_ = 0;
    statNumWriteThroughs_ = 0;

    // Store gate ids
    inGateId_ = gate("in")->id();

    // Initialize derived cache implementations
    initializeCache();
}

void BufferCache::finish()
{
    // Finalize derived cache implementations
    finalizeCache();

    // Record statistics
    double totalReads = double(statNumHits_) + double(statNumMisses_);
    double statHitRate = (double)statNumHits_ / totalReads;
    recordScalar("SPFS Buffer Cache Requests", statNumRequests_);
    recordScalar("SPFS Buffer Cache Write Through", statNumWriteThroughs_);
    recordScalar("SPFS Buffer Cache Hits", statNumHits_);
    recordScalar("SPFS Buffer Cache Misses", statNumMisses_);
    recordScalar("SPFS Buffer Cache Hit Rate", statHitRate);
}


void BufferCache::handleMessage(cMessage *msg)
{
    //  Either the message is a request of the cache or it is a response
    //  to a disk access request made as the result of a miss.
    if ( msg->arrivalGateId() == inGateId_ )
    {
        statNumRequests_++;
        handleBlockRequest(msg);
    }
    else
    {
        handleBlockResponse(msg);
    }
}

void BufferCache::registerHit()
{
    statNumHits_++;
}

void BufferCache::registerMiss()
{
    statNumMisses_++;
}

void BufferCache::registerWriteThrough()
{
    statNumWriteThroughs_++;
}

//=============================================================================
//
// NoBufferCache implementation (concrete BufferCache)
//
//=============================================================================
Define_Module(NoBufferCache)

NoBufferCache::NoBufferCache()
{
}

void NoBufferCache::initializeCache()
{
}

void NoBufferCache::finalizeCache()
{
}

void NoBufferCache::handleBlockRequest(cMessage* blockRequest)
{
    // Forward request to next module
    send(blockRequest, "request");
}

void NoBufferCache::handleBlockResponse(cMessage* blockResponse)
{
    // Forward response to next module
    send(blockResponse, "out");
}

//=============================================================================
//
// LRUBufferCache implementation (concrete BufferCache)
//
//=============================================================================
Define_Module(LRUBufferCache)

LRUBufferCache::LRUBufferCache()
    : cache_(0)
{
}

void LRUBufferCache::initializeCache()
{
    dirtyThreshold_ = par("dirtyThreshold");

    long numEntries = par("numEntries");
    cache_ = new LRUCache<LogicalBlockAddress, char>(numEntries);
}

void LRUBufferCache::finalizeCache()
{
    assert(0 != cache_);
    delete cache_;
}

void LRUBufferCache::handleBlockRequest(cMessage* msg)
{
    if (spfsOSReadDeviceRequest* read =
        dynamic_cast<spfsOSReadDeviceRequest*>(msg))
    {
        if (isCached(read->getAddress()))
        {
            // Update statistics
            registerHit();

            // Create and send response
            spfsOSReadDeviceResponse* resp = new spfsOSReadDeviceResponse();
            resp->setContextPointer(msg);
            send(resp, "out");
        }
        else
        {
            // Update statistics
            registerMiss();

            // Forward request to next module
            send(msg, "request");
        }
    }
    else if (spfsOSWriteDeviceRequest* write =
             dynamic_cast<spfsOSWriteDeviceRequest*>(msg))
    {
        // If the cache has too many dirty entries, switch to write-through
        if (cache_->percentDirty() > dirtyThreshold_)
        {
            registerWriteThrough();
            write->setWriteThrough(true);
        }

        // Retrieve the write through status
        bool isWriteThrough = write->getWriteThrough();

        // Perform cache eviction if needed
        evictCacheEntry(write->getAddress());

        // Add an entry to the cache with dirty status determined by write
        // through status
        LogicalBlockAddress writeLBA = write->getAddress();
        bool isDirty = !isWriteThrough;
        cache_->insert(writeLBA, 0, isDirty);

        // Perform write though if necessary
        if (isWriteThrough)
        {
            // Forward write request to device
            send(write, "request");
        }
        else
        {
            // Create and send response
            spfsOSWriteDeviceResponse* resp = new spfsOSWriteDeviceResponse();
            resp->setContextPointer(msg);
            send(resp, "out");
        }
    }
    else
    {
        cerr << "Buffer Cache Error: Invalid message received." << endl;
    }
}

void LRUBufferCache::handleBlockResponse(cMessage* msg)
{
    // Add data read from the disk to the cache
    cMessage* req = static_cast<cMessage*>(msg->contextPointer());
    if (spfsOSReadDeviceRequest* read =
        dynamic_cast<spfsOSReadDeviceRequest*>(req))
    {
        LogicalBlockAddress lba = read->getAddress();

        // If a cache entry does not exist for this block, then no later
        // write has arrived and the returned value is valid to cache
        if (!isCached(lba))
        {
            // Perform cache eviction if needed
            evictCacheEntry(lba);

            // Add block to cache
            cache_->insert(lba, 0, false);
        }

        // Forward completed response up the chain
        send( msg, "out" );
    }
    else if (spfsOSWriteDeviceRequest* write =
             dynamic_cast<spfsOSWriteDeviceRequest*>(req))
    {
        bool isWriteThrough = write->getWriteThrough();
        if (isWriteThrough)
        {
            // Create and send response for write through request
            spfsOSWriteDeviceResponse* resp = new spfsOSWriteDeviceResponse();
            resp->setContextPointer(req);
            send(resp, "out");
            delete msg;
        }
        else
        {
            // Discard dirty block write back request and response
            delete req;
            delete msg;
        }
    }

}

void LRUBufferCache::evictCacheEntry(LogicalBlockAddress lba)
{
    if (isFull() && !isCached(lba))
    {
        Entry evictee = getNextEviction();
        if (evictee.isDirty)
        {
            spfsOSWriteDeviceRequest* write = new spfsOSWriteDeviceRequest();
            write->setAddress(evictee.lba);
            send(write, "request");
        }
    }
}

bool LRUBufferCache::isCached(LogicalBlockAddress address)
{
    if (cache_->exists(address))
    {
        // Use lookup to refresh the LRU ordering for this entry
        cache_->lookup(address);
        return true;
    }
    else
    {
        return false;
    }
}

bool LRUBufferCache::isFull()
{
    return (cache_->capacity() == cache_->size());
}

LRUBufferCache::Entry LRUBufferCache::getNextEviction()
{
    // Retrieve the LRU item to be evicted
    pair<LogicalBlockAddress,char> lruEntry = cache_->getLRU();

    Entry evictee;
    evictee.lba = lruEntry.first;
    evictee.isDirty = cache_->getDirtyBit(evictee.lba);
    return evictee;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
