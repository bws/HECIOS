//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "multi_cache.h"
#include <cassert>
#include <utility>
using namespace std;

bool operator<(const MultiCache::Key& lhs, const MultiCache::Key& rhs)
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

MultiCache::MultiCache(int capacity)
    : maxEntries_(capacity)
{
    assert(0 < maxEntries_);
}

MultiCache::~MultiCache()
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


void MultiCache::insertFullPageAndRecall(const Key& key,
                                         const Page& fullPage,
                                         bool isDirty,
                                         Key& outEvictedKey,
                                         Page*& outEvictedPage,
                                         bool& outEvictedDirtyBit)
{
    // If this is a dirty full page insertion, we can get rid of any existing
    // copies safely
    if (isDirty)
    {
        remove(key);
    }

    int count = keyEntryMap_.count(key);
    assert(count <= 2);

    if (0 == count)
    {
        // Perform the insertion
        EntryType* entry = new EntryType();
        entry->page = new Page(fullPage);
        entry->isDirty = isDirty;

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
    }
    else
    {
        // This is a full page insertion so we only need to
        // update the LRU status for the existing pages
        lruList_.push_front(key);
        std::pair<MapType::iterator, MapType::iterator> range;
        range = keyEntryMap_.equal_range(key);
        bool erased = false;
        while (range.first != range.second)
        {
            if (!erased)
            {
                lruList_.erase(range.first->second->lruRef);
                erased = true;
            }
            range.first->second->lruRef = lruList_.begin();
            range.first++;
        }
    }

    // If the cache is full, retrieve and perform the next eviction
    bool hasEviction = false;
    if (maxEntries_ < keyEntryMap_.size())
    {
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

void MultiCache::insertDirtyPartialPageAndRecall(const Key& key,
                                                 const PartialPage& partialPage,
                                                 Key& outEvictedKey,
                                                 Page*& outEvictedPage,
                                                 bool& outEvictedDirtyBit)
{
    // A dirty partial page can occur in 3 cases.  The dirty full page exists,
    // The clean page exists, or the clean and dirty page exist

    // Determine the number of pages in existence
    size_t count = keyEntryMap_.count(key);
    assert(1 == count || 2 == count);

    if (count == 1)
    {
        MapType::iterator pos = keyEntryMap_.find(key);

        // Update the LRU list for the existing page
        lruList_.erase(pos->second->lruRef);
        lruList_.push_front(key);
        pos->second->lruRef = lruList_.begin();

        if (!pos->second->isDirty)
        {
            // Insert the dirty partial page alongside the clean page
            EntryType* entry = new EntryType();
            entry->page = new PartialPage(partialPage);
            entry->isDirty = true;
            entry->lruRef = lruList_.begin();

            // Insert the cache entry
            keyEntryMap_.insert(std::make_pair(key, entry));
        }
    }
    else
    {
        lruList_.push_front(key);

        // Update the LRU for both pages
        pair<MapType::iterator, MapType::iterator> range;
        range = keyEntryMap_.equal_range(key);
        MapType::iterator iter = range.first;
        MapType::iterator last = range.second;
        while (iter != last)
        {
            if (iter->second->isDirty)
            {
                // Merge the file regions together
                FileRegionSet::iterator frsIter = partialPage.regions.begin();
                FileRegionSet::iterator frsEnd = partialPage.regions.end();
                while (frsIter != frsEnd)
                {
                    PartialPage* existingPage =
                        dynamic_cast<PartialPage*>(iter->second->page);
                    assert(0 != existingPage);
                    existingPage->regions.insert(*frsIter);
                    frsIter++;
                }

                // Remove the old LRU for the partial page, this must
                // be done once, so just do it for the dirty copy
                lruList_.erase(iter->second->lruRef);
            }

            // Update the LRU
            iter->second->lruRef = lruList_.begin();
            iter++;
        }
    }

    // If the cache was full, retrieve and perform the next eviction
    bool hasEviction = false;
    if (maxEntries_ < keyEntryMap_.size())
    {
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

void MultiCache::remove(const Key& key)
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
        delete iter->second->page;
        delete iter->second;

        // Remove from the map
        keyEntryMap_.erase(iter++);
    }
}

bool MultiCache::exists(const Key& key) const
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

std::vector<MultiCache::Page*> MultiCache::lookup(const Key& key)
{
    std::vector<Page*> values;

    // Search the map for key
    std::pair<MapType::iterator, MapType::iterator> range;
    range = keyEntryMap_.equal_range(key);
    MapType::iterator first = range.first;
    MapType::iterator last = range.second;
    if (first != last)
    {
        lruList_.push_front(key);
        bool erased = false;
        while (first != last)
        {
            // Retrieve the value
            EntryType* entry = first->second;
            values.push_back(entry->page);

            // Remove the LRU ref exactly once
            if (!erased)
            {
                lruList_.erase(entry->lruRef);
                erased = true;
            }

            // Refresh the LRU list
            entry->lruRef = lruList_.begin();

            // Move to the next element for this key
            first++;
        }
    }
    else
    {
        NoSuchEntry e;
        throw e;
    }
    return values;
}

std::vector<MultiCache::Page*> MultiCache::getDirtyEntries() const
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

std::vector<MultiCache::Page*> MultiCache::getFilteredEntries(
    const MultiCache::FilterFunctor& filterFunc) const
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

std::size_t MultiCache::capacity() const
{
    return maxEntries_;
}

std::size_t MultiCache::size() const
{
    return keyEntryMap_.size();
}

void MultiCache::performEviction(Key& outEvictedKey,
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

void MultiCache::print(ostream& ost) const
{
    ost << "MultiCache Contents" << endl;
    MapType::const_iterator iter = keyEntryMap_.begin();
    MapType::const_iterator last = keyEntryMap_.end();
    while (iter != last)
    {
        ost << "\t";
        ost << "[" << iter->first.filename << "," << iter->first.key;
        ost << " -> ";
        ost << "[" << iter->second->page->id << "," << iter->second->isDirty << "]";
        ost << endl;
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
