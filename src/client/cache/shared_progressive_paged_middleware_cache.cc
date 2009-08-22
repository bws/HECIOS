//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include "progressive_paged_middleware_cache.h"
using namespace std;

class SharedProgressivePagedMiddlewareCache : public ProgressivePagedMiddlewareCache
{
public:
    /** Constructor */
    SharedProgressivePagedMiddlewareCache();

protected:
    /** Perform module initialization */
    virtual void initialize();

    /** @return the file data page cache for this middleware */
    virtual FileDataPageCache* createFileDataPageCache(size_t cacheSize);

    /** @return the map of pending reads indexed by request */
    virtual RequestMap* createPendingDataMap();

    /**
     * @return the map of the number of times each file has been opened for
     *   this middleware
     */
    virtual OpenFileMap* createOpenFileMap();

private:
    /** @ return The shared resource */
    template<class SharedResource>
    SharedResource* createSharedResource(std::map<cModule*, SharedResource*>& sharedResourceMap);

    /** @return the parent compute node for this middleware */
    cModule* findParentComputeNode() const;

    /** Type for storing caches indexed by the compute node */
    typedef std::map<cModule*, ProgressivePagedMiddlewareCache::FileDataPageCache*> SharedCacheMap;

    /** Type for storing requests indexed by compute node */
    typedef std::map<cModule*, ProgressivePagedMiddlewareCache::RequestMap*> SharedRequestMap;

    /** Type for storing open file counts indexed by compute node */
    typedef std::map<cModule*, ProgressivePagedMiddlewareCache::OpenFileMap*> SharedOpenFileMap;

    /** The current cached file data for each compute node cache */
    static SharedCacheMap sharedCacheMap_;

    /** The current outstanding requests for each compute node cache */
    static SharedRequestMap sharedPendingRequestMap_;

    /** The current open files for each compute node cache */
    static SharedOpenFileMap sharedOpenFileMap_;

};

// OMNet Registration Method
Define_Module(SharedProgressivePagedMiddlewareCache);

// Static variable initialization
SharedProgressivePagedMiddlewareCache::SharedCacheMap
    SharedProgressivePagedMiddlewareCache::sharedCacheMap_;

// Static variable initialization
SharedProgressivePagedMiddlewareCache::SharedRequestMap
    SharedProgressivePagedMiddlewareCache::sharedPendingRequestMap_;

// Static variable initialization
SharedProgressivePagedMiddlewareCache::SharedOpenFileMap
    SharedProgressivePagedMiddlewareCache::sharedOpenFileMap_;

SharedProgressivePagedMiddlewareCache::SharedProgressivePagedMiddlewareCache()
{
}

void SharedProgressivePagedMiddlewareCache::initialize()
{
    // Initialize parent
    ProgressivePagedMiddlewareCache::initialize();
}

ProgressivePagedMiddlewareCache::FileDataPageCache*
SharedProgressivePagedMiddlewareCache::createFileDataPageCache(size_t cacheSize)
{
    // Retrieve the compute node model
    cModule* cpun = findParentComputeNode();

    // If a cache for this node exists, return it
    // Otherwise, create it
    SharedProgressivePagedMiddlewareCache::FileDataPageCache* cache = 0;
    SharedCacheMap::iterator iter = sharedCacheMap_.find(cpun);
    if (iter != sharedCacheMap_.end())
    {
        cache = iter->second;
        assert(cacheSize == cache->capacity());
    }
    else
    {
        cache = new SharedProgressivePagedMiddlewareCache::FileDataPageCache(cacheSize);
        sharedCacheMap_[cpun] = cache;
    }
    return cache;
}

ProgressivePagedMiddlewareCache::RequestMap*
SharedProgressivePagedMiddlewareCache::createPendingDataMap()
{
    return createSharedResource(sharedPendingRequestMap_);
}

ProgressivePagedMiddlewareCache::OpenFileMap*
SharedProgressivePagedMiddlewareCache::createOpenFileMap()
{
    return createSharedResource(sharedOpenFileMap_);
}

template<class SharedResource>
SharedResource*
SharedProgressivePagedMiddlewareCache::createSharedResource(map<cModule*,
                                                            SharedResource*>& sharedResourceMap)
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

cModule* SharedProgressivePagedMiddlewareCache::findParentComputeNode() const
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
