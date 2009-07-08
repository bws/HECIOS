//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "shared_direct_paged_middleware_cache.h"
#include <cassert>
using namespace std;

// OMNet Registriation Method
Define_Module(SharedDirectPagedMiddlewareCache);

// Static variable initialization
SharedDirectPagedMiddlewareCache::SharedCacheMap
    SharedDirectPagedMiddlewareCache::sharedCacheMap_;

// Static variable initialization
SharedDirectPagedMiddlewareCache::SharedRequestMap
    SharedDirectPagedMiddlewareCache::sharedPendingRequestMap_;

// Static variable initialization
SharedDirectPagedMiddlewareCache::SharedOpenFileMap
    SharedDirectPagedMiddlewareCache::sharedOpenFileMap_;

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
    // Retrieve the compute node model
    cModule* cpun = findParentComputeNode();

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
    return createSharedResource(sharedPendingRequestMap_);
}

DirectPagedMiddlewareCache::OpenFileMap*
SharedDirectPagedMiddlewareCache::createOpenFileMap()
{
    return createSharedResource(sharedOpenFileMap_);
}

template<class SharedResource>
SharedResource*
SharedDirectPagedMiddlewareCache::createSharedResource(map<cModule*, SharedResource*>& sharedResourceMap)
{
    typedef map<cModule*, SharedResource*> SharedResourceMap;

    // Retrieve the compute node model
    cModule* cpun = findParentComputeNode();

    // If a shared resource for this node exists, return it
    // Otherwise, create and return it
    SharedResource* sharedResource = 0;
    typename SharedResourceMap::iterator iter = sharedResourceMap.find(cpun);
    if (iter != sharedResourceMap.end())
    {
        sharedResource = iter->second;
    }
    else
    {
        sharedResource = new SharedResource();
        sharedResourceMap[cpun] = sharedResource;
    }
    return sharedResource;
}

cModule* SharedDirectPagedMiddlewareCache::findParentComputeNode() const
{
    // Extract the compute node model
    cModule* mpiProcess = parentModule();
    assert(0 != mpiProcess);
    cModule* jobProcess = mpiProcess->parentModule();
    assert(0 != jobProcess);
    cModule* cpun = jobProcess->parentModule();
    assert(0 != cpun);
    return cpun;
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
