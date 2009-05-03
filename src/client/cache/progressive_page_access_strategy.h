#ifndef PROGRESSIVE_PAGE_ACCESS_STRATEGY_H_
#define PROGRESSIVE_PAGE_ACCESS_STRATEGY_H_

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
#include "progressive_paged_middleware_cache.h"
class Filename;
class FileDescriptor;
class FileView;
class spfsMPIFileReadAtRequest;
class spfsMPIFileWriteAtRequest;

/** Class for writing progressive cache pages to disk */
class ProgressivePageAccessStrategy
{
public:
    /** Convenience typedef */
    typedef ProgressivePagedMiddlewareCache::ProgressivePage ProgressivePage;

    /** Write entries */
    typedef ProgressivePagedMiddlewareCache::WritebackPage WritebackPage;

    /** The pages for a file partitioned into full pages and partial pages */
    struct FilePages
    {
        std::set<ProgressivePage> fullPages;
        std::set<ProgressivePage> partialPages;
    };

    /** Pages sorted by filename */
    typedef std::map<Filename, FilePages> FilePageMap;

    /** Default constructor */
    ProgressivePageAccessStrategy(FSSize pageSize);

    /** Destructor */
    virtual ~ProgressivePageAccessStrategy();

    /** @return the page size */
    FSSize getPageSize() const;

    /** @return a request to write the desired pages */
    virtual std::vector<spfsMPIFileWriteAtRequest*> createPFSWriteRequests(
        const std::vector<WritebackPage>& entries,
        spfsMPIFileRequest* origRequest) const;

private:

    /** Copy constructor hidden */
    ProgressivePageAccessStrategy(const ProgressivePageAccessStrategy& other);

    /** Assignment operator hidden */
    ProgressivePageAccessStrategy& operator=(const ProgressivePageAccessStrategy& other);

    /** @return group the pages into groups by filename */
    void groupPagesByFilename(
        const std::vector<WritebackPage>& pageKeys,
        FilePageMap& outPageGroups) const;

    /** @return a descriptor with a view of only the selected pages */
    FileDescriptor* getPageViewDescriptor(
        const Filename& filename,
        const std::set<ProgressivePage>& pages) const;

    /** @return a descriptor with a view of only the selected regions */
    FileDescriptor* getRegionViewDescriptor(
        const Filename& filename,
        const std::set<ProgressivePage>& pages) const;

    /** Page size */
    FSSize pageSize_;
};

#endif /* PROGRESSIVE_PAGE_ACCESS_STRATEGY_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
