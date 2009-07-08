#ifndef PROGRESSIVE_PAGE_CACHE_H
#define PROGRESSIVE_PAGE_CACHE_H
//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include "file_page.h"
#include "dirty_file_region_set.h"
#include "filename.h"
#include "spfs_exceptions.h"

/**
 * A cache that allows multiple entries for a single page
 */
class ProgressivePageCache
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

     /** A progressive cache page */
    struct Page
    {
        FilePageId id;
        DirtyFileRegionSet regions;
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
    typedef std::map<Key, EntryType*> MapType;

    /** Convenience typedef of the lru list */
    typedef std::list<Key> LRUListType;

    /**
     * Constructor
     */
    ProgressivePageCache(int capacity);

    /** Destructor */
    ~ProgressivePageCache();

    /**
     * Perform the cache insertion and set the evicted value and dirty bit
     * in the outbound parameters.
     *
     * @throw NoSuchEntry if no eviction occurs
     */
    void insertPageAndRecall(const Key& key,
                             const Page& page,
                             bool isDirty,
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
    Page* lookup(const Key& key);

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

    /** Print cache contents into ost */
    virtual void print(std::ostream& ost) const;

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
bool operator<(const ProgressivePageCache::Key& lhs, const ProgressivePageCache::Key& rhs);

/** @return true if the lhs key is ordered before the rhs key */
bool operator<(const ProgressivePageCache::Page& lhs, const ProgressivePageCache::Page& rhs);

/** Add progressive cache to the ostream */
inline std::ostream& operator<<(std::ostream& ost, const ProgressivePageCache& cache)
{
    cache.print(ost);
    return ost;
}

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
