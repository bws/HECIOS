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
#include "shared_direct_paged_middleware_cache.h"
#include <cassert>
using namespace std;

// Static variable initialization
SharedDirectPagedMiddlewareCache::SharedCacheMap
    SharedDirectPagedMiddlewareCache::sharedCacheMap_;

// Static variable initialization
SharedDirectPagedMiddlewareCache::SharedRequestMap
    SharedDirectPagedMiddlewareCache::sharedPendingRequestMap_;

SharedDirectPagedMiddlewareCache::SharedDirectPagedMiddlewareCache()
{

}

void SharedDirectPagedMiddlewareCache::initialize()
{
    DirectPagedMiddlewareCache::initialize();
}

DirectPagedMiddlewareCache::FileDataPageCache*
SharedDirectPagedMiddlewareCache::createFileDataPageCache(size_t cacheSize)
{
    // Extract the compute node model
    cModule* mpiProcess = parentModule();
    assert(0 != mpiProcess);
    cModule* jobProcess = mpiProcess->parentModule();
    assert(0 != jobProcess);
    cModule* cpun = jobProcess->parentModule();
    assert(0 != cpun);

    // If a cache for this node exists, return it
    // Otherwise, create it
    DirectPagedMiddlewareCache::FileDataPageCache* cache = 0;
    SharedCacheMap::iterator iter = sharedCacheMap_.find(cpun);
    if (iter != sharedCacheMap_.end())
    {
        cache = iter->second;
        assert(cacheSize == cache->capacity());
    }
    else
    {
        cache = new DirectPagedMiddlewareCache::FileDataPageCache(cacheSize);
        sharedCacheMap_[cpun] = cache;
    }
    return cache;
}

DirectPagedMiddlewareCache::RequestMap*
SharedDirectPagedMiddlewareCache::createPendingRequestMap()
{
    // Extract the compute node model
    cModule* mpiProcess = parentModule();
    assert(0 != mpiProcess);
    cModule* jobProcess = mpiProcess->parentModule();
    assert(0 != jobProcess);
    cModule* cpun = jobProcess->parentModule();
    assert(0 != cpun);

    // If a request map for this node exists, return it
    // Otherwise, create it
    DirectPagedMiddlewareCache::RequestMap* pendingRequests = 0;
    SharedRequestMap::iterator iter = sharedPendingRequestMap_.find(cpun);
    if (iter != sharedPendingRequestMap_.end())
    {
        pendingRequests = iter->second;
    }
    else
    {
        pendingRequests = new DirectPagedMiddlewareCache::RequestMap();
        sharedPendingRequestMap_[cpun] = pendingRequests;
    }
    return pendingRequests;

}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
