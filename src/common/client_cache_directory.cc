//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "client_cache_directory.h"
#include "data_type_layout.h"
#include "data_type_processor.h"
#include "file_builder.h"
#include "file_page_utils.h"
#include "file_view.h"
#include "file_distribution.h"
using namespace std;

ClientCacheDirectory::ClientCacheDirectory()
{

}

ClientCacheDirectory::~ClientCacheDirectory()
{

}

ClientCacheDirectory::InvalidationMap
ClientCacheDirectory::getClientsNeedingInvalidate(const Filename& filename,
                                                  const FSSize& pageSize,
                                                  const FSOffset& offset,
                                                  const FSSize& dataSize,
                                                  const FileView& view,
                                                  const FileDistribution& dist) const
{
    // Get the file regions accessed
    FSMetaData* meta = FileBuilder::instance().getMetaData(filename);
    int serverIdx = dist.getObjectIdx();
    DataTypeLayout dataLayout;
    DataTypeProcessor::createServerFileLayoutForRead(offset,
                                                     dataSize,
                                                     view,
                                                     dist,
                                                     meta->bstreamSizes[serverIdx],
                                                     dataLayout);

    // Translate the regions into page ids
    FilePageUtils& pageUtils = FilePageUtils::instance();
    set<FilePageId> pageIds = pageUtils.regionsToPageIds(pageSize,
                                                         dataLayout.getRegions());

    // Determine the caches containing these pages
    InvalidationMap invalidations;
    set<FilePageId>::iterator iter = pageIds.begin();
    while (iter != pageIds.end())
    {
        Entry entry = {filename, *(iter++)};
        pair<CacheEntryToClientMap::const_iterator,
             CacheEntryToClientMap::const_iterator> range;
        range = clientCacheEntries_.equal_range(entry);

        CacheEntryToClientMap::const_iterator j;
        for (j = range.first; j != range.second; ++j)
        {
            invalidations[j->second].insert(entry);
        }
    }
    return invalidations;
}

void ClientCacheDirectory::addClientCacheEntry(const ClientCache& client,
                                               const Filename& filename,
                                               const FilePageId& pageId,
                                               State state)
{
    const Entry entry = {filename, pageId, state};
    clientCacheEntries_.insert(make_pair(entry, client));
}

void ClientCacheDirectory::removeClientCacheEntry(const ClientCache& client,
                                                  const Filename& filename,
                                                  const FilePageId& pageId)
{
    pair<CacheEntryToClientMap::iterator, CacheEntryToClientMap::iterator> range;
    const Entry entry = {filename, pageId};
    range = clientCacheEntries_.equal_range(entry);

    CacheEntryToClientMap::iterator iter = range.first;
    while (iter != range.second)
    {
        if (client == iter->second)
        {
            clientCacheEntries_.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

void ClientCacheDirectory::removeClientCacheEntryByRank(int rank,
                                                        const Filename& filename,
                                                        const FilePageId& pageId)
{
    pair<CacheEntryToClientMap::iterator, CacheEntryToClientMap::iterator> range;
    const Entry entry = {filename, pageId};
    range = clientCacheEntries_.equal_range(entry);

    CacheEntryToClientMap::iterator iter = range.first;
    while (iter != range.second)
    {
        if (rank == iter->second.rank)
        {
            clientCacheEntries_.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
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
