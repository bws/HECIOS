//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include <cstddef>
#include <map>
#include "paged_middleware_cache_with_twin.h"
using namespace std;

/** A Direct paged cache for a single node */
class SharedPagedMiddlewareCacheWithTwin : public PagedMiddlewareCacheWithTwin
{
public:
    /** Constructor */
    SharedPagedMiddlewareCacheWithTwin();

protected:
    /** Perform module initialization */
    virtual void initialize();

    /** @return the file data cache for this compute node */
    virtual PagedMiddlewareCacheWithTwin::FileDataPageCache* createFileDataPageCache(std::size_t cacheSize);

    /** @return The pending request map for this compute node */
    virtual PagedMiddlewareCacheWithTwin::RequestMap* createPendingRequestMap();

    /** @return The pending request map for this compute node */
    virtual PagedMiddlewareCacheWithTwin::PartialRequestMap* createPendingPartialRequestMap();

    /** @return The open file map for this compute node */
    virtual PagedMiddlewareCacheWithTwin::OpenFileMap* createOpenFileMap();

private:
    /** @ return The shared resource */
    template<class SharedResource>
    SharedResource* createSharedResource(std::map<cModule*, SharedResource*>& sharedResourceMap);

    /** @return the parent compute node for this middleware */
    cModule* findParentComputeNode() const;

    /** Type for storing caches indexed by the compute node */
    typedef std::map<cModule*, PagedMiddlewareCacheWithTwin::FileDataPageCache*> SharedCacheMap;

    /** Type for storing requests indexed by compute node */
    typedef std::map<cModule*, PagedMiddlewareCacheWithTwin::RequestMap*> SharedRequestMap;

    /** Type for storing requests indexed by compute node */
    typedef std::map<cModule*, PagedMiddlewareCacheWithTwin::PartialRequestMap*> SharedPartialRequestMap;

    /** Type for storing open file counts indexed by compute node */
    typedef std::map<cModule*, PagedMiddlewareCacheWithTwin::OpenFileMap*> SharedOpenFileMap;

    /** The current cached file data for each compute node cache */
    static SharedCacheMap sharedCacheMap_;

    /** The current outstanding requests for each compute node cache */
    static SharedRequestMap sharedPendingRequestMap_;

    /** The current outstanding requests for each compute node cache */
    static SharedPartialRequestMap sharedPendingPartialRequestMap_;

    /** The current open files for each compute node cache */
    static SharedOpenFileMap sharedOpenFileMap_;
};

// OMNet Registriation Method
Define_Module(SharedPagedMiddlewareCacheWithTwin);

// Static variable initialization
SharedPagedMiddlewareCacheWithTwin::SharedCacheMap
    SharedPagedMiddlewareCacheWithTwin::sharedCacheMap_;

// Static variable initialization
SharedPagedMiddlewareCacheWithTwin::SharedRequestMap
    SharedPagedMiddlewareCacheWithTwin::sharedPendingRequestMap_;

// Static variable initialization
SharedPagedMiddlewareCacheWithTwin::SharedPartialRequestMap
    SharedPagedMiddlewareCacheWithTwin::sharedPendingPartialRequestMap_;

// Static variable initialization
SharedPagedMiddlewareCacheWithTwin::SharedOpenFileMap
    SharedPagedMiddlewareCacheWithTwin::sharedOpenFileMap_;

SharedPagedMiddlewareCacheWithTwin::SharedPagedMiddlewareCacheWithTwin()
{

}

void SharedPagedMiddlewareCacheWithTwin::initialize()
{
    PagedMiddlewareCacheWithTwin::initialize();
}

PagedMiddlewareCacheWithTwin::FileDataPageCache*
SharedPagedMiddlewareCacheWithTwin::createFileDataPageCache(size_t cacheSize)
{
    // Retrieve the compute node model
    cModule* cpun = findParentComputeNode();

    // If a cache for this node exists, return it
    // Otherwise, create it
    PagedMiddlewareCacheWithTwin::FileDataPageCache* cache = 0;
    SharedCacheMap::iterator iter = sharedCacheMap_.find(cpun);
    if (iter != sharedCacheMap_.end())
    {
        cache = iter->second;
        assert(cacheSize == cache->capacity());
    }
    else
    {
        cache = new PagedMiddlewareCacheWithTwin::FileDataPageCache(cacheSize);
        sharedCacheMap_[cpun] = cache;
    }
    return cache;
}

PagedMiddlewareCacheWithTwin::RequestMap*
SharedPagedMiddlewareCacheWithTwin::createPendingRequestMap()
{
    return createSharedResource(sharedPendingRequestMap_);
}

PagedMiddlewareCacheWithTwin::PartialRequestMap*
SharedPagedMiddlewareCacheWithTwin::createPendingPartialRequestMap()
{
    return createSharedResource(sharedPendingPartialRequestMap_);
}

PagedMiddlewareCacheWithTwin::OpenFileMap*
SharedPagedMiddlewareCacheWithTwin::createOpenFileMap()
{
    return createSharedResource(sharedOpenFileMap_);
}

template<class SharedResource>
SharedResource*
SharedPagedMiddlewareCacheWithTwin::createSharedResource(map<cModule*, SharedResource*>& sharedResourceMap)
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

cModule* SharedPagedMiddlewareCacheWithTwin::findParentComputeNode() const
{
    // Extract the compute node model
    cModule* mpiProcess = getParentModule();
    assert(0 != mpiProcess);
    cModule* jobProcess = mpiProcess->getParentModule();
    assert(0 != jobProcess);
    cModule* cpun = jobProcess->getParentModule();
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
