#ifndef PAGED_CACHE_H_
#define PAGED_CACHE_H_

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
#include <set>
#include "basic_data_type.h"
#include "file_page.h"
#include "filename.h"
#include "middleware_cache.h"
class FileDescriptor;
class FileView;
class spfsMPIFileRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileWriteAtRequest;

/** An abstract page aligned cache */
class PagedCache : public MiddlewareCache
{
public:
    /** Key used to locate a page in the cache */
    class Key
    {
    public:
        /** Constructor */
        Key(const Filename& fn, std::size_t k) : filename(fn), key(k) {};

        /** Filename to store data in cache for */
        Filename filename;

        /** Key to identify a page (usually just the page id) */
        std::size_t key;
    };

    /** Constructor */
    PagedCache();

    /** Abstract destructor */
    virtual ~PagedCache() = 0;

    /** @return the number of pages the cache can contain */
    size_t cacheCapacity() const { return pageCapacity_; };

    /** @return the cache's page size */
    size_t pageSize() const { return pageSize_; };

    /** @return the first offset for the page */
    FSOffset pageBeginOffset(const FilePageId& pageId) const;

    /** @return Array of pages ids spanning the supplied file regions */
    std::set<FilePageId> determineRequestPages(const FSOffset& offset,
                                               const FSSize& size,
                                               const FileView& view);

    /**
     * @return Array of pages ids that are fully covered by
     *         the request (no partial pages)
     */
    std::set<FilePageId> determineRequestFullPages(const FSOffset& offset,
                                                   const FSSize& size,
                                                   const FileView& view);

    /**
     * @return Array of pages ids that are only partially covered by
     *         the request
     */
    std::set<FilePageId> determineRequestPartialPages(const FSOffset& offset,
                                                      const FSSize& size,
                                                      const FileView& view);

protected:
    /** Module initialization */
    void initialize();

    /** @return a request to read the desired pages */
    spfsMPIFileReadAtRequest* createPageReadRequest(
        const Filename& filename,
        const std::set<FilePageId>& pageIds,
        spfsMPIFileRequest* origRequest) const;

    /** @return a request to write the desired pages */
    spfsMPIFileWriteAtRequest* createPageWriteRequest(
        const Filename& filename,
        const std::set<FilePageId>& pageIds,
        spfsMPIFileRequest* origRequest) const;

private:
    /** @return Array of page ids spanning the supplied file regions */
    std::set<FilePageId> regionsToPageIds(
        const std::vector<FileRegion>& fileRegions);

    /** @return Array of pages spanning the supplied file regions */
    std::set<FilePage> regionsToPages(
        const std::vector<FileRegion>& fileRegions);

    /** @return a file descriptor for filename with the page view applied */
    FileDescriptor* getPageViewDescriptor(
        const Filename& filename,
        const std::set<FilePageId>& pageIds) const;

    /** Page size attribute */
    std::size_t pageSize_;

    /** Page capacity attribute */
    std::size_t pageCapacity_;

    /** Byte Data Type used within cache pages */
    ByteDataType byteDataType_;
};

/** @return true if the lhs key is ordered before the rhs key */
bool operator<(const PagedCache::Key& lhs, const PagedCache::Key& rhs);



#endif /* PAGED_CACHE_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
