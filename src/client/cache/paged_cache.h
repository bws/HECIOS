#ifndef PAGED_CACHE_H_
#define PAGED_CACHE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <set>
#include "basic_data_type.h"
#include "file_page.h"
#include "filename.h"
#include "middleware_cache.h"
class FileDescriptor;
class FileRegionSet;
class FileView;
class spfsMPIFileRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileWriteAtRequest;

/** An abstract page aligned cache */
class PagedCache : public MiddlewareCache
{
public:
    /** Key used to locate a page in the cache */
    struct Key
    {
    public:
        /** Constructor */
        Key(const Filename& fn, std::size_t k) : filename(fn), key(k) {};

        /** Filename to store data in cache for */
        Filename filename;

        /** Key to identify a page (usually just the page id) */
        std::size_t key;
    };

    /** Sets of pages currently being read or written */
    struct InProcessPages
    {
    public:
        /** Pages in the process of being read shared */
        std::set<PagedCache::Key> readPages;

        /** Pages in the process of being read exclusive*/
        std::set<PagedCache::Key> readExclusivePages;

        /** Pages in the process of being written */
        std::set<PagedCache::Key> writePages;
    };

    /** @return the Cache Keys for the page set */
    static std::set<PagedCache::Key> convertPagesToCacheKeys(const Filename& filename,
                                                             std::set<FilePageId> pageIds);

    /** @return the Page Ids for filename in the set of cache keys */
    static std::set<FilePageId> convertCacheKeysToPages(const Filename& filename,
                                                        std::set<PagedCache::Key> keys);

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
                                               const FileView& view) const;

    /**
     * @return Array of pages ids that are fully covered by
     *         the request (no partial pages)
     */
    std::set<FilePageId> determineRequestFullPages(const FSOffset& offset,
                                                   const FSSize& size,
                                                   const FileView& view) const;

    /**
     * @return Array of pages ids that are only partially covered by
     *         the request
     */
    std::set<FilePageId> determineRequestPartialPages(const FSOffset& offset,
                                                      const FSSize& size,
                                                      const FileView& view) const;

    /**
     * @return The file regions residing on the partial page
     */
    FileRegionSet determinePartialPageRegions(const FilePageId& pageId,
                                              const FSOffset& offset,
                                              const FSSize& size,
                                              const FileView& view) const;

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

/** Print set of paged cache keys */
std::ostream& operator<<(std::ostream& ost, const std::set<PagedCache::Key>& keys);

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
