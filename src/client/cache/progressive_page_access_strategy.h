#ifndef PROGRESSIVE_PAGE_ACCESS_STRATEGY_H_
#define PROGRESSIVE_PAGE_ACCESS_STRATEGY_H_
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
