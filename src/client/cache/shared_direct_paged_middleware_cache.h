#ifndef SHARED_DIRECT_PAGED_MIDDLEWARE_CACHE_H_
#define SHARED_DIRECT_PAGED_MIDDLEWARE_CACHE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <map>
#include "direct_paged_middleware_cache.h"

/** A Direct paged cache for a single node */
class SharedDirectPagedMiddlewareCache : public DirectPagedMiddlewareCache
{
public:
    /** Constructor */
    SharedDirectPagedMiddlewareCache();

protected:
    /** Perform module initialization */
    virtual void initialize();

    /** @return the file data cache for this compute node */
    virtual DirectPagedMiddlewareCache::FileDataPageCache* createFileDataPageCache(std::size_t cacheSize);

    /** @return The pending request map for this compute node */
    virtual DirectPagedMiddlewareCache::RequestMap* createPendingRequestMap();

    /** @return The open file map for this compute node */
    virtual DirectPagedMiddlewareCache::OpenFileMap* createOpenFileMap();

private:
    /** @ return The shared resource */
    template<class SharedResource>
    SharedResource* createSharedResource(std::map<cModule*, SharedResource*>& sharedResourceMap);

    /** @return the parent compute node for this middleware */
    cModule* findParentComputeNode() const;

    /** Type for storing caches indexed by the compute node */
    typedef std::map<cModule*, DirectPagedMiddlewareCache::FileDataPageCache*> SharedCacheMap;

    /** Type for storing requests indexed by compute node */
    typedef std::map<cModule*, DirectPagedMiddlewareCache::RequestMap*> SharedRequestMap;

    /** Type for storing open file counts indexed by compute node */
    typedef std::map<cModule*, DirectPagedMiddlewareCache::OpenFileMap*> SharedOpenFileMap;

    /** The current cached file data for each compute node cache */
    static SharedCacheMap sharedCacheMap_;

    /** The current outstanding requests for each compute node cache */
    static SharedRequestMap sharedPendingRequestMap_;

    /** The current open files for each compute node cache */
    static SharedOpenFileMap sharedOpenFileMap_;
};


#endif /* SHARED_DIRECT_PAGED_MIDDLEWARE_CACHE_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
