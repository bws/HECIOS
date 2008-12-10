#ifndef SHARED_DIRECT_PAGED_MIDDLEWARE_CACHE_H_
#define SHARED_DIRECT_PAGED_MIDDLEWARE_CACHE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2008 Bradley W. Settlemyer
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

    /** @return The writeback count file map for this compute node */
    virtual DirectPagedMiddlewareCache::OpenFileMap* createWritebackCountMap();

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

    /** Type for storing open file counts indexed by compute node */
    typedef std::map<cModule*, DirectPagedMiddlewareCache::WritebackCountMap*> SharedWritebackCountMap;

    /** The current cached file data for each compute node cache */
    static SharedCacheMap sharedCacheMap_;

    /** The current outstanding requests for each compute node cache */
    static SharedRequestMap sharedPendingRequestMap_;

    /** The current open files for each compute node cache */
    static SharedOpenFileMap sharedOpenFileMap_;

    /** The current open files for each compute node cache */
    static SharedWritebackCountMap sharedWritebackCountMap_;

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
