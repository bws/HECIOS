//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "progressive_page_cache.h"
#include <cassert>
#include <utility>
using namespace std;

bool operator<(const ProgressivePageCache::Key& lhs, const ProgressivePageCache::Key& rhs)
{
    if (lhs.filename == rhs.filename)
    {
        return (lhs.key < rhs.key);
    }
    else
    {
        return (lhs.filename < rhs.filename);
    }
}

bool operator<(const ProgressivePageCache::Page& lhs, const ProgressivePageCache::Page& rhs)
{
    return (lhs.id < rhs.id);
}

ProgressivePageCache::ProgressivePageCache(int capacity)
    : maxEntries_(capacity)
{
    assert(0 < maxEntries_);
}

ProgressivePageCache::~ProgressivePageCache()
{
    // Delete any EntryTypes still contained in the map
    MapType::iterator iter;
    for (iter = keyEntryMap_.begin(); iter != keyEntryMap_.end(); ++iter)
    {
        delete iter->second;
    }

    // Clear containers
    lruList_.clear();
    keyEntryMap_.clear();
}


void ProgressivePageCache::insertPageAndRecall(const Key& key,
                                               const Page& page,
                                               bool isDirty,
                                               Key& outEvictedKey,
                                               Page*& outEvictedPage,
                                               bool& outEvictedDirtyBit)
{
    // If the key exists, merge this page and don't evict
    int count = keyEntryMap_.count(key);
    assert(count <= 1);

    if (1 == count)
    {
        // Locate the old entry
        MapType::iterator entry = keyEntryMap_.find(key);

        // Refresh the lru status
        lruList_.erase(entry->second->lruRef);
        lruList_.push_front(key);
        entry->second->lruRef = lruList_.begin();

        // Merge in the changes to the region set
        Page* page = entry->second->page;
        DirtyFileRegionSet::iterator newIter = page->regions.begin();
        DirtyFileRegionSet::iterator newEnd = page->regions.end();
        while (newIter != newEnd)
        {
            // Insert dirty page region into the set
            page->regions.insert(*newIter);
            ++newIter;
        }

        // If this insertion is dirty, make sure the page is marked dirty
        if (isDirty)
        {
            entry->second->isDirty = true;
        }
    }
    else
    {
        // Perform the insertion
        EntryType* entry = new EntryType();
        entry->page = new Page(page);
        entry->isDirty = isDirty;

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
    }

    // If the cache is full, retrieve and perform the next eviction
    bool hasEviction = false;
    if (maxEntries_ < keyEntryMap_.size())
    {
        assert(0 == count);
        assert((maxEntries_ + 1) == keyEntryMap_.size());
        // Perform the eviction
        performEviction(outEvictedKey, outEvictedPage, outEvictedDirtyBit);

        // Indicate an eviction occurred
        hasEviction = true;
    }

    if (!hasEviction)
    {
        NoSuchEntry e;
        throw e;
    }
}

void ProgressivePageCache::remove(const Key& key)
{
    std::pair<MapType::iterator, MapType::iterator> range = keyEntryMap_.equal_range(key);
    MapType::iterator iter = range.first;
    bool lruErased = false;
    while (iter != range.second)
    {
        // Cleanup the lru list
        if (!lruErased)
        {
            lruList_.erase(iter->second->lruRef);
            lruErased = true;
        }

        // Cleanup the EntryType memory
        delete iter->second;

        // Remove from the map
        keyEntryMap_.erase(iter++);
    }
}

bool ProgressivePageCache::exists(const Key& key) const
{
    multimap<Key, EntryType*>::const_iterator pos;

    // Search the map for key
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        return true;
    }
    return false;
}

ProgressivePageCache::Page* ProgressivePageCache::lookup(const Key& key)
{
    Page* page = 0;
    // Search the map for key
    MapType::iterator iter = keyEntryMap_.find(key);
    if (iter != keyEntryMap_.end())
    {
        // Retrieve the value
        EntryType* entry = iter->second;
        page = entry->page;

        // Refresh the LRU list
        lruList_.erase(entry->lruRef);
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();
    }
    else
    {
        NoSuchEntry e;
        throw e;
    }
    return page;
}

std::vector<ProgressivePageCache::Page*> ProgressivePageCache::getDirtyEntries() const
{
    std::vector<Page*> dirtyEntries;

    // Loop thru the map to find all of the dirty entries
    MapType::const_iterator mapEnd = keyEntryMap_.end();
    MapType::const_iterator mapIter;
    for (mapIter = keyEntryMap_.begin(); mapIter != mapEnd; mapIter++)
    {
        if (mapIter->second->isDirty)
        {
            dirtyEntries.push_back(mapIter->second->page);
        }
    }

    return dirtyEntries;
}

std::vector<ProgressivePageCache::Page*> ProgressivePageCache::getFilteredEntries(
    const FilterFunctor& filterFunc) const
{
    std::vector<Page*> filteredEntries;

    // Loop thru the map to find all of the dirty entries
    MapType::const_iterator mapEnd = keyEntryMap_.end();
    MapType::const_iterator mapIter;
    for (mapIter = keyEntryMap_.begin(); mapIter != mapEnd; mapIter++)
    {
        if (filterFunc(mapIter->first, *mapIter->second))
        {
            filteredEntries.push_back(mapIter->second->page);
        }
    }

    return filteredEntries;
}

std::size_t ProgressivePageCache::capacity() const
{
    return maxEntries_;
}

std::size_t ProgressivePageCache::size() const
{
    return keyEntryMap_.size();
}

void ProgressivePageCache::performEviction(Key& outEvictedKey,
                                           Page*& outEvictedPage,
                                           bool& outEvictedDirtyBit)
{
    assert(keyEntryMap_.size() > maxEntries_);

    // Get the LRU item
    Key lruKey = *(lruList_.rbegin());

    // Set the outbound parameters
    std::pair<MapType::iterator, MapType::iterator> range;
    range = keyEntryMap_.equal_range(lruKey);
    MapType::iterator iter = range.first;
    MapType::iterator last = range.second;

    // Set the eviction to the first entry
    outEvictedKey = iter->first;
    outEvictedPage = iter->second->page;
    outEvictedDirtyBit = iter->second->isDirty;

    // If there is a dirty entry, overwrite the retrieved values
    while (iter != last)
    {
        if (iter->second->isDirty)
        {
            outEvictedKey = iter->first;
            outEvictedPage = iter->second->page;
            outEvictedDirtyBit = iter->second->isDirty;

            // Page ownership is now transferred, remove the page reference
            iter->second->page = 0;
            break;
        }
        iter++;
    }
    assert(0 != outEvictedPage);

    // Remove the entry/entries
    remove(lruKey);
}

void ProgressivePageCache::print(ostream& ost) const
{
    ost << "ProgressivePageCache Contents" << endl;
    MapType::const_iterator iter = keyEntryMap_.begin();
    MapType::const_iterator last = keyEntryMap_.end();
    while (iter != last)
    {
        ost << "\t"
            << "[" << iter->first.filename << "," << iter->first.key
            << " -> "
            << "[" << iter->second->page->id << ","
            << "[" << iter->second->page->regions << ","
            << iter->second->isDirty << "]"
            << endl;
        iter++;
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
