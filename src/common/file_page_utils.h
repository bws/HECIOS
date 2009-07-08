#ifndef FILE_PAGE_UTILS_H_
#define FILE_PAGE_UTILS_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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

    /** @return a pointer to afile view of only the requested pages */
    FileView* createPageViewDescriptor(const FSSize& pageSize,
                                       const std::set<FilePageId>& pageIds) const;

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
