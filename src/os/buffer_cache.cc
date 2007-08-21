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
        else
        {
            cerr << "Buffer Cache Error: Invalid message received." << endl;
        }
    }
    else
    {
        // Add data back from the disk to the cache
        LogicalBlockAddress lba;
        cMessage* req = static_cast<cMessage*>(msg->contextPointer());
        if (spfsOSReadDeviceRequest* read =
            dynamic_cast<spfsOSReadDeviceRequest*>(req))
        {
            lba = read->getAddress();
        }
        else if (spfsOSWriteDeviceRequest* write =
                 dynamic_cast<spfsOSWriteDeviceRequest*>(req))
        {
            lba = write->getAddress();
        }
        else
        {
            assert(0);
        }

        // Add block to cache
        addEntry(lba);

        // Forward completed response up the chain
        send( msg, "out" );
    }
}

//=============================================================================
//
// NoBufferCache implementation (concrete BufferCache)
//
//=============================================================================
Define_Module_Like( NoBufferCache, BufferCache )

NoBufferCache::NoBufferCache()
{
}

bool NoBufferCache::isCached(LogicalBlockAddress address)
{
    return false;
}

//=============================================================================
//
// LRUBufferCache implementation (concrete BufferCache)
//
//=============================================================================
Define_Module_Like(LRUBufferCache, BufferCache)

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

void LRUBufferCache::addEntry(LogicalBlockAddress address)
{
    assert(false == cache_->exists(address));
    cache_->insert(address, 1);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
