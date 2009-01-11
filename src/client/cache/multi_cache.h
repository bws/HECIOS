#ifndef MULTI_CACHE_H
#define MULTI_CACHE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
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
#include <functional>
#include <list>
#include <map>
#include <vector>
#include "file_page.h"
#include "file_region_set.h"
#include "filename.h"
#include "spfs_exceptions.h"

/**
 * A cache that allows multiple entries for a single page
 */
class MultiCache
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

    /** A basic cache page */
    struct Page
    {
        virtual ~Page() {};

        FilePageId id;
    };

    /** A partial cache page */
    struct PartialPage : public Page
    {
        virtual ~PartialPage() {};

        FileRegionSet regions;
    };

    /** A cache entry */
    struct EntryType
    {
        Page* page;
        bool isDirty;
        std::list<Key>::iterator lruRef;
    };

    /**
     * Function object that filter cache entries based on the key
     */
    struct FilterFunctor : std::binary_function<Key, EntryType, bool>
    {
        bool operator()(const Key& key, const EntryType& entry) const
        {
            return filter(key, *(entry.page), entry.isDirty);
        }

        virtual bool filter(const Key& key, const Page& val, bool isDirty) const = 0;
    };

    /** Convenience typedef of the key-value map */
    typedef std::multimap<Key, EntryType*> MapType;

    /** Convenience typedef of the lru list */
    typedef std::list<Key> LRUListType;

    /**
     * Constructor
     */
    MultiCache(int capacity);

    /** Destructor */
    ~MultiCache();

    /**
     * Perform the cache insertion and set the evicted value and dirty bit
     * in the outbound parameters.
     *
     * @throw NoSuchEntry if no eviction occurs
     */
    void insertFullPageAndRecall(const Key& key,
                                 const Page& page,
                                 bool isDirty,
                                 Key& outEvictedKey,
                                 Page*& outEvictedPage,
                                 bool& outEvictedDirtyBit);

    /**
     * Perform the cache insertion and set the evicted value and dirty bit
     * in the outbound parameters.
     *
     * @throw NoSuchEntry if no eviction occurs
     */
    void insertDirtyPartialPageAndRecall(const Key& key,
                                         const PartialPage& partialPage,
                                         Key& outEvictedKey,
                                         Page*& outEvictedPage,
                                         bool& outEvictedDirtyBit);

    /**
     * Remove the value(s) for key from the cache
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    void remove(const Key& key);

    /**
     * @return The values for key and update the LRU ordering
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    std::vector<Page*> lookup(const Key& key);

    /**
     * Check if an entry exists without modifying its LRU status
     *
     * @return true if a value exists for the given key
     */
    bool exists(const Key& key) const;

    /**
     * @return a vector of all the dirty cache values.  Does not update
     *         the LRU status
     */
    std::vector<Page*> getDirtyEntries() const;

    /**
     * @return a vector of all the cache keys that the filter returns
     *   true for.
     */
    std::vector<Page*> getFilteredEntries(const FilterFunctor& filter) const;

    /**
     * @return the cache capacity
     */
    std::size_t capacity() const;

    /**
     * @return the number of entries in the cache
     */
    std::size_t size() const;

protected:
    /**
     * Remove the lru entry and return the clean entry if only it exists,
     * or return the dirty entry if both a clean and dirty entry exist
     */
    void performEviction(Key& outEvictedKey,
                         Page*& outEvictedPage,
                         bool& outEvictedDirtyBit);

private:

    MapType keyEntryMap_;
    LRUListType lruList_;

    const std::size_t maxEntries_;
};

/** @return true if the lhs key is ordered before the rhs key */
bool operator<(const MultiCache::Key& lhs, const MultiCache::Key& rhs);

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
