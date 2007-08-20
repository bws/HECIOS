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

#include "buffer_cache.h"
#include "os_proto_m.h"
using namespace std;

//------------------------------------------------
//
//  BufferCache
//
//  This is the base class for all Cache Management Algorithms.
//  All cache algorithm implementations manage a single set of
//  cache elements.

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
        //  Data is back from the disk        
        send( msg, "out" );
    }
}

//------------------------------------------------
//
//  NoBufferCache
//
//  The No Cache Management Algorithm simulates having no cache at all.
//  It says that the requested element is not in the cache.
Define_Module_Like( NoBufferCache, BufferCache )

NoBufferCache::NoBufferCache()
{
}

bool NoBufferCache::isCached(LogicalBlockAddress address)
{
    return false;
}

//------------------------------------------------
//
//  LRUBufferCache
//
//  The LRU Cache Management Algorithm chooses the element
//  in the cache that was requested the longest time ago
//  for replacement regardless of its priority.

Define_Module_Like(LRUBufferCache, BufferCache)

LRUBufferCache::LRUBufferCache()
{
}

bool LRUBufferCache::isCached(LogicalBlockAddress address)
{
    return true;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
