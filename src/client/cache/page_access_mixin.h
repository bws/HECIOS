#ifndef PAGE_ACCESS_MIXIN_H_
#define PAGE_ACCESS_MIXIN_H_

//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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
#include <set>
#include <vector>
#include "basic_data_type.h"
#include "basic_types.h"
#include "file_page.h"
#include "paged_cache.h"
class Filename;
class FileDescriptor;
class FileView;
class spfsCacheReadExclusiveRequest;
class spfsCacheReadSharedRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileWriteAtRequest;

/** Abstract base class for behavior that reads and writes cache pages */
class PageAccessMixin
{
public:
    /** */
    static std::size_t getByteLength(const FileView& view);

    /** Default constructor */
    PageAccessMixin();

    /** Constructor */
    PageAccessMixin(FSSize pageSize);

    /** Destructor */
    virtual ~PageAccessMixin() = 0;

    /** @return the page size */
    FSSize getPageSize() const;

    /** Set the page size */
    void setPageSize(const FSSize& pageSize);

    /** @return the read exclusive requests for this set of pages */
    virtual std::vector<spfsCacheReadExclusiveRequest*> createCacheReadExclusiveRequests(
        int cacheRank,
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* parentRequest) const = 0;

    /** @return the read shared requests for this set of pages */
    virtual std::vector<spfsCacheReadSharedRequest*> createCacheReadSharedRequests(
        int cacheRank,
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* parentRequest) const = 0;

    /** @return a request to read the desired pages */
    virtual std::vector<spfsMPIFileReadAtRequest*> createPFSReadRequests(
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* origRequest) const = 0;

    /** @return a request to write the desired pages */
    virtual std::vector<spfsMPIFileWriteAtRequest*> createPFSWriteRequests(
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* origRequest) const = 0;

private:
    /** Copy constructor hidden */
    PageAccessMixin(const PageAccessMixin& other);

    /** Assignment operator hidden */
    PageAccessMixin& operator=(const PageAccessMixin& other);

    /** Page size */
    FSSize pageSize_;
};

/** Mixin behavior that reads and writes cache pages a single page at a time */
class SinglePageAccessMixin : public PageAccessMixin
{
public:
    /** Default constructor */
    SinglePageAccessMixin();

    /** */
    ~SinglePageAccessMixin();

    /** @return the read exclusive requests for this set of pages */
    virtual std::vector<spfsCacheReadExclusiveRequest*> createCacheReadExclusiveRequests(
        int cacheRank,
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* parentRequest) const;

    /** @return the read shared requests for this set of pages */
    virtual std::vector<spfsCacheReadSharedRequest*> createCacheReadSharedRequests(
        int cacheRank,
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* parentRequest) const;

    /** @return a request to read the desired pages */
    virtual std::vector<spfsMPIFileReadAtRequest*> createPFSReadRequests(
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* origRequest) const;

    /** @return a request to write the desired pages */
    virtual std::vector<spfsMPIFileWriteAtRequest*> createPFSWriteRequests(
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* origRequest) const;

private:
    /** Copy constructor hidden */
    SinglePageAccessMixin(const PageAccessMixin& other);

    /** Assignment operator hidden */
    SinglePageAccessMixin& operator=(const PageAccessMixin& other);
};

/** Mixin behavior that reads and writes multiple cache pages per request */
class BlockIndexedPageAccessMixin : public PageAccessMixin
{
public:
    /** Default constructor */
    BlockIndexedPageAccessMixin();

    /** Constructor */
    BlockIndexedPageAccessMixin(FSSize pageSize);

    /** Destructor */
    ~BlockIndexedPageAccessMixin();

    /** @return the read exclusive requests for this set of pages */
    virtual std::vector<spfsCacheReadExclusiveRequest*> createCacheReadExclusiveRequests(
        int cacheRank,
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* parentRequest) const;

    /** @return the read shared requests for this set of pages */
    virtual std::vector<spfsCacheReadSharedRequest*> createCacheReadSharedRequests(
        int cacheRank,
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* parentRequest) const;

    /** @return a request to read the desired pages */
    virtual std::vector<spfsMPIFileReadAtRequest*> createPFSReadRequests(
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* origRequest) const;

    /** @return a request to write the desired pages */
    virtual std::vector<spfsMPIFileWriteAtRequest*> createPFSWriteRequests(
        const std::set<PagedCache::Key>& pages,
        spfsMPIFileRequest* origRequest) const;

protected:
    /** */
    typedef std::map<Filename, std::set<FilePageId> > FilePageMap;

    /** @return group the pages into groups by filename */
    void groupPagesByFilename(const std::set<PagedCache::Key>& pageKeys,
                              FilePageMap& outPageGroups) const;

private:
    /** Copy constructor hidden */
    BlockIndexedPageAccessMixin(const PageAccessMixin& other);

    /** Assignment operator hidden */
    BlockIndexedPageAccessMixin& operator=(const PageAccessMixin& other);

    /** @return a block indexed view of the file for the pages */
    FileDescriptor* getPageViewDescriptor(const Filename& filename,
                                          const std::set<FilePageId>& pageIds) const;
};

#endif /* PAGE_ACCESS_MIXIN_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
