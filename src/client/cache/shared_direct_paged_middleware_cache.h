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

    virtual DirectPagedMiddlewareCache::FileDataPageCache* createFileDataPageCache(std::size_t cacheSize);

    virtual DirectPagedMiddlewareCache::RequestMap* createPendingRequestMap();

private:
    typedef std::map<cModule*, DirectPagedMiddlewareCache::FileDataPageCache*> SharedCacheMap;

    typedef std::map<cModule*, DirectPagedMiddlewareCache::RequestMap*> SharedRequestMap;

    /** The current cached file data for each compute node cache */
    static SharedCacheMap sharedCacheMap_;

    /** The current outstanding requests for each compute node cache */
    static SharedRequestMap sharedPendingRequestMap_;
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
