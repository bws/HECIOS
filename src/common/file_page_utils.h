#ifndef FILE_PAGE_UTILS_H_
#define FILE_PAGE_UTILS_H_

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
#include <map>
#include <set>
#include <vector>
#include "basic_types.h"
#include "file_page.h"
#include "file_region_set.h"
#include "singleton.h"
class FileDistribution;
class FileView;

class FilePageUtils : public Singleton<FilePageUtils>
{
public:
    /** Enable singleton construction */
    friend class Singleton<FilePageUtils>;

    /** @return the cache's page size */
    //size_t pageSize() const { return pageSize_; };

    /** @return the first offset for the page */
    FSOffset pageBeginOffset(const FSSize& pageSize,
                             const FilePageId& pageId) const;

    /** @return Array of pages ids spanning the supplied file regions */
    std::set<FilePageId> determineRequestPages(const FSSize& pageSize,
                                               const FSOffset& offset,
                                               const FSSize& size,
                                               const FileView& view) const;

    /**
     * @return Array of pages ids spanning the supplied file regions that
     *   are located *WITHIN* the supplied distribution
     */
    std::set<FilePageId> determineRequestPages(const FSSize& pageSize,
                                               const FSOffset& offset,
                                               const FSSize& size,
                                               const FileView& view,
                                               const FileDistribution& dist) const;

    /**
     * @return Array of pages ids that are fully covered by
     *         the request (no partial pages)
     */
    std::set<FilePageId> determineRequestFullPages(const FSSize& pageSize,
                                                   const FSOffset& offset,
                                                   const FSSize& size,
                                                   const FileView& view) const;

    /**
     * @return Array of pages ids that are only partially covered by
     *         the request
     */
    std::set<FilePageId> determineRequestPartialPages(const FSSize& pageSize,
                                                      const FSOffset& offset,
                                                      const FSSize& size,
                                                      const FileView& view) const;

    /**
     * @return The file regions residing on the partial page
     */
    FileRegionSet determinePartialPageRegions(const FSSize& pageSize,
                                              const FilePageId& pageId,
                                              const FSOffset& offset,
                                              const FSSize& size,
                                              const FileView& view) const;

    /** @return Array of pages spanning the supplied file regions */
    std::set<FilePage> regionsToPages(const FSSize& pageSize,
                                      const std::vector<FileRegion>& fileRegions) const;

    /** @return Array of page ids spanning the supplied file regions */
    std::set<FilePageId> regionsToPageIds(const FSSize& pageSize,
                                          const std::vector<FileRegion>& fileRegions) const;

private:
    /** Private constructor */
    FilePageUtils();

    /** Private destructor */
    ~FilePageUtils();

    /** Hidden copy constructor */
    FilePageUtils(const FilePageUtils& other);

    /** Hidden assignment operator */
    FilePageUtils& operator=(const FilePageUtils& other);
};


#endif /* FILE_PAGE_UTILS_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
