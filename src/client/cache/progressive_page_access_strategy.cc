//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "progressive_page_access_strategy.h"
#include <cassert>
#include "basic_data_type.h"
#include "block_indexed_data_type.h"
#include "cache_proto_m.h"
#include "filename.h"
#include "file_builder.h"
#include "file_descriptor.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
using namespace std;

//
// Implementation for Progressive Page Access Strategy
//
ProgressivePageAccessStrategy::ProgressivePageAccessStrategy(FSSize pageSize)
    : pageSize_(pageSize)
{
    assert(pageSize_ > 0);
}

ProgressivePageAccessStrategy::~ProgressivePageAccessStrategy()
{
}

FSSize ProgressivePageAccessStrategy::getPageSize() const
{
    return pageSize_;
}


vector<spfsMPIFileWriteAtRequest*>
ProgressivePageAccessStrategy::createPFSWriteRequests(
    const vector<WritebackPage>& pages,
    spfsMPIFileRequest* parentRequest) const
{
    // Group the pages
    FilePageMap pagesByName;
    groupPagesByFilename(pages, pagesByName);

    // Create the requests for each file
    vector<spfsMPIFileWriteAtRequest*> requests;
    FilePageMap::iterator iter = pagesByName.begin();
    FilePageMap::iterator last = pagesByName.end();
    static DataType* byteType = new ByteDataType();
    while (iter != last)
    {
        FilePages& filePages = iter->second;
        // Write the complete pages
        if (!filePages.fullPages.empty())
        {
            // Construct a descriptor that views only the full pages
            FileDescriptor* fd = getPageViewDescriptor(iter->first, iter->second.fullPages);

            // Create the write request
            spfsMPIFileWriteAtRequest* fullPagesWrite =
                new spfsMPIFileWriteAtRequest("Progressive FP Write Request",
                                              SPFS_MPI_FILE_WRITE_AT_REQUEST);
            fullPagesWrite->setContextPointer(parentRequest);
            fullPagesWrite->setFileDes(fd);
            fullPagesWrite->setOffset(0);
            fullPagesWrite->setDataType(byteType);
            fullPagesWrite->setCount(iter->second.fullPages.size() * pageSize_);
            requests.push_back(fullPagesWrite);
        }

        // Write the partial pages
        if (!filePages.partialPages.empty())
        {
            // Construct a descriptor that views only the full pages
            FileDescriptor* fd = getRegionViewDescriptor(iter->first, iter->second.partialPages);

            // Create the write request
            spfsMPIFileWriteAtRequest* partialPagesWrite =
                new spfsMPIFileWriteAtRequest("Progressive PP Write Request",
                                              SPFS_MPI_FILE_WRITE_AT_REQUEST);
            partialPagesWrite->setContextPointer(parentRequest);
            partialPagesWrite->setFileDes(fd);
            partialPagesWrite->setOffset(0);
            partialPagesWrite->setDataType(byteType);
            partialPagesWrite->setCount(1);
            requests.push_back(partialPagesWrite);
        }

        // Increment iterator
        ++iter;
    }
    return requests;
}

void ProgressivePageAccessStrategy::groupPagesByFilename(
    const vector<WritebackPage>& pageKeys,
    FilePageMap& outPageGroups) const
{
    assert(0 == outPageGroups.size());
    vector<WritebackPage>::const_iterator iter = pageKeys.begin();
    vector<WritebackPage>::const_iterator last = pageKeys.end();
    while (iter != last)
    {
        FilePages& pages = outPageGroups[iter->filename];
        ProgressivePage p = {iter->id, iter->regions};

        // Determine whether page is full or partial
        if (pageSize_ == iter->regions.numBytes() &&
            1 == iter->regions.size())
        {
            pages.fullPages.insert(p);
        }
        else
        {
            pages.partialPages.insert(p);
        }

        ++iter;
    }
}

FileDescriptor* ProgressivePageAccessStrategy::getPageViewDescriptor(
    const Filename& filename,
    const set<ProgressivePage>& pageIds) const
{
    assert(!pageIds.empty());

    // Create the vector of displacements
    set<ProgressivePage>::const_iterator idIter = pageIds.begin();
    vector<size_t> displacements(pageIds.size());
    for (size_t i = 0; i < displacements.size(); i++)
    {
        displacements[i] = pageSize_ * idIter->id;
        ++idIter;
    }

    // Create the block indexed data type to use as the view
    static ByteDataType byteDataType;
    BlockIndexedDataType* pageView = new BlockIndexedDataType(pageSize_,
                                                              displacements,
                                                              byteDataType);
    FileView cacheView(0, pageView);

    // Create a descriptor with which to apply the view
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(filename);
    fd->setFileView(cacheView);
    return fd;
}

FileDescriptor* ProgressivePageAccessStrategy::getRegionViewDescriptor(
    const Filename& filename,
    const set<ProgressivePage>& pageIds) const
{
    assert(!pageIds.empty());

    // Create the vector of displacements
    set<ProgressivePage>::const_iterator idIter = pageIds.begin();
    set<ProgressivePage>::const_iterator idEnd = pageIds.end();
    vector<size_t> blockLengths;
    vector<size_t> displacements;
    while (idIter != idEnd)
    {
        DirtyFileRegionSet::const_iterator iter = idIter->regions.begin();
        DirtyFileRegionSet::const_iterator last = idIter->regions.end();
        while (iter != last)
        {
            if (iter->isDirty)
            {
                displacements.push_back(iter->offset);
                blockLengths.push_back(iter->extent);
            }
            ++iter;
        }
        ++idIter;
    }
    assert(!blockLengths.empty());
    assert(!displacements.empty());
    assert(blockLengths.size() == displacements.size());

    // Create the block indexed data type to use as the view
    static ByteDataType byteDataType;
    IndexedDataType* regionView = new IndexedDataType(blockLengths,
                                                      displacements,
                                                      byteDataType);
    FileView cacheView(0, regionView);

    // Create a descriptor with which to apply the view
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(filename);
    fd->setFileView(cacheView);
    return fd;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
