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

    // Store gate ids
    inGateId_ = gate("in")->id();

    // Initialize derived cache implementations
    initializeCache();
}

void BufferCache::finish()
{
    double statHitRate = (double)statNumHits_ / (double) statNumRequests_;
    recordScalar("Buffer Cache Requests", statNumRequests_);
    recordScalar("Buffer Cache Hits", statNumHits_);
    recordScalar("Buffer Cache Misses", statNumMisses_);
    recordScalar("Buffer Cache Hit Rate", statHitRate);
}


void BufferCache::handleMessage(cMessage *msg)
{
    //  Either the message is a request of the cache or it is a response
    //  to a disk access request made as the result of a miss.
    if ( msg->arrivalGateId() == inGateId_ )
    {
        handleBlockRequest(msg);
    }
    else
    {
        handleBlockResponse(msg);
    }
}

void BufferCache::handleBlockRequest(cMessage* msg)
{
    if (spfsOSReadDeviceRequest* read =
        dynamic_cast<spfsOSReadDeviceRequest*>(msg))
    {
        // Update statistics
        statNumRequests_++;
        
        if (isCached(read->getAddress()))
        {
            // Update statistics
            statNumHits_++;

            // Create and send response
            spfsOSReadDeviceResponse* resp =
                new spfsOSReadDeviceResponse();
            resp->setContextPointer(msg);
            send(resp, "out"); 
        }
        else
        {
            // Update statistics
            statNumMisses_++;

            // Forward request to next module
            send(msg, "request");
        }
    }
    else if (spfsOSWriteDeviceRequest* write =
             dynamic_cast<spfsOSWriteDeviceRequest*>(msg))
    {
        // Perform cache eviction if needed
        evictCacheEntry(write->getAddress());
            
        // Add a dirty entry to the cache
        Entry newEntry;
        newEntry.lba = write->getAddress();
        newEntry.isDirty = true;
        insertEntry(newEntry);

        // Create and send response
        spfsOSWriteDeviceResponse* resp = new spfsOSWriteDeviceResponse();
        resp->setContextPointer(msg);
        send(resp, "out"); 
    }
    else
    {
        cerr << "Buffer Cache Error: Invalid message received." << endl;
    }
}

void BufferCache::handleBlockResponse(cMessage* msg)
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
            Entry newEntry;
            newEntry.lba = lba;
            newEntry.isDirty = false;
            insertEntry(newEntry);
        }

        // Forward completed response up the chain
        send( msg, "out" );
    }
    else
    {
        // Discard dirty block write back responses
        delete msg;
    }
}

void BufferCache::evictCacheEntry(LogicalBlockAddress lba)
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

bool NoBufferCache::isFull()
{
    return false;
}

bool NoBufferCache::isCached(LogicalBlockAddress address)
{
    return false;
}

void NoBufferCache::insertEntry(const BufferCache::Entry& newEntry)
{
}

BufferCache::Entry NoBufferCache::getNextEviction()
{
    logic_error e("Illegal to invoke getNextEviction");
    throw e;
    Entry* null = 0;
    return *null;
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
    // Remove the cache if one exists
    if (0 != cache_)
        delete cache_;
    
    long long gigabyte = 1073741824;
    long long blockSize = 512;
    long long numEntries = 4 * gigabyte / blockSize;
    cache_ = new LRUCache<LogicalBlockAddress, int>(numEntries);
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

void LRUBufferCache::insertEntry(const BufferCache::Entry& entry)
{
    cache_->insert(entry.lba, entry.isDirty);
}

BufferCache::Entry LRUBufferCache::getNextEviction()
{
    // Retrieve the LRU item to be evicted
    pair<LogicalBlockAddress,bool> lruEntry = cache_->getLRU();
    
    Entry evictee;
    evictee.lba = lruEntry.first;
    evictee.isDirty = lruEntry.second;
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
