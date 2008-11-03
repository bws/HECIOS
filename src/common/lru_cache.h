#ifndef LRU_CACHE_H
#define LRU_CACHE_H
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

#include <cassert>
#include <list>
#include <map>
#include <utility>
#include <vector>
#include "spfs_exceptions.h"
/**
 * A simple fixed size cache using LRU replacement.
 */
template <class KeyType, class ValueType>
class LRUCache
{
public:

    /**
     * A cache entry for
     */
    struct EntryType
    {
        ValueType data;
        bool isDirty;
        typename std::list<KeyType>::iterator lruRef;
    };

    /** Convencience typedef of the key-value map */
    typedef std::map<KeyType,EntryType*> MapType;

    /** Convencience typedef of the lru list */
    typedef std::list<KeyType> LRUListType;

    /**
     * Constructor
     */
    LRUCache(int capacity);

    /** Destructor */
    ~LRUCache();

    /**
     * Insert a Key-Value pair into the cache.  If the cache already
     * contains more entries than the maximum size, evict the least
     * recently used item
     *
     * @param key the cache lookup key
     * @param value the cached value
     * @param isDirty the cached value's dirty bit
     */
    void insert(const KeyType& key,
                const ValueType& value,
                bool isDirty = false);

    /**
     * Perform the cache insertion and set the evicted value and dirty bit
     * in the outbound parameters.
     *
     * @throw NoSuchEntry if no eviction occurs
     */
    void insertAndRecall(const KeyType& key,
                         const ValueType& value,
                         bool isDirty,
                         KeyType& outEvictedKey,
                         ValueType& outEvictedValue,
                         bool& outEvictedDirtyBit);

    /**
     * Remove the value for key from the cache
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    void remove(const KeyType& key);

    /**
     * Set the dirty-bit for the cache key in question without updating
     * the LRU status
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    void setDirtyBit(const KeyType& key, bool dirtyValue);

    /**
     * @return The value for key and update the LRU ordering
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    ValueType lookup(const KeyType& key);

    /**
     * Check if an entry exists without modifying its LRU status
     *
     * @return true if a value exists for the given key
     */
    bool exists(const KeyType& key) const;

    /**
     * @return the dirty bit for key
     *
     * @throw NoSuchENtry if no entry exists for the key
     */
    bool getDirtyBit(const KeyType& key) const;

    /**
     * @return a vector of all the dirty cache values.  Does not update
     *         the LRU status
     */
    std::vector<KeyType> getDirtyEntries() const;

    /**
     * @return the next entry that will be evicted on a new insertion
     *
     * @throw NoSuchEntry if the cache is empty
     */
    std::pair<KeyType, ValueType> getLRU() const;

    /**
     * @return the cache capacity
     */
    std::size_t capacity() const;

    /**
     * @return the number of entries in the cache
     */
    std::size_t size() const;

    /** @return the percentage of the cache capacity that is dirty */
    double percentDirty() const;

private:

    std::map<KeyType, EntryType*> keyEntryMap_;
    std::list<KeyType> lruList_;

    const std::size_t maxEntries_;
    std::size_t numDirtyEntries_;
    std::size_t numEntries_;
};

template <class KeyType, class ValueType>
LRUCache<KeyType,ValueType>::LRUCache(int capacity)
    : maxEntries_(capacity),
      numDirtyEntries_(0),
      numEntries_(0)
{
    assert(0 < maxEntries_);
}

template <class KeyType, class ValueType>
LRUCache<KeyType,ValueType>::~LRUCache()
{
    // Delete any EntryTypes still contained in the map
    typename std::map<KeyType, EntryType*>::iterator iter;
    for (iter = keyEntryMap_.begin(); iter != keyEntryMap_.end(); ++iter)
    {
        delete iter->second;
    }

    // Clear containers
    lruList_.clear();
    keyEntryMap_.clear();
}


template<class KeyType, class ValueType>
void LRUCache<KeyType,ValueType>::insert(const KeyType& key,
                                         const ValueType& value,
                                         bool isDirty)
{
    // Check to see if the entry already exists
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Update the number of dirty entries if necessary
        if (isDirty && !pos->second->isDirty)
        {
            numDirtyEntries_++;
        }

        // Entry already exists, update it
        pos->second->data = value;
        pos->second->isDirty = isDirty;

        // Update the LRU data
        lruList_.erase(pos->second->lruRef);
        lruList_.push_front(key);
        pos->second->lruRef = lruList_.begin();
    }
    else
    {
        // If the cache is full, evict an item according to LRU policy
        if (numEntries_ == maxEntries_)
        {
            KeyType lruKey = *(lruList_.rbegin());
            this->remove(lruKey);
        }

        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->data = value;
        entry->isDirty = isDirty;

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
        numEntries_++;

        // Update the number of dirty entries if necessary
        if (isDirty)
        {
            numDirtyEntries_++;
        }
    }
}

template<class KeyType, class ValueType>
void LRUCache<KeyType,ValueType>::insertAndRecall(const KeyType& key,
                                                  const ValueType& value,
                                                  bool isDirty,
                                                  KeyType& outEvictedKey,
                                                  ValueType& outEvictedValue,
                                                  bool& outEvictedDirtyBit)
{
    bool hasEviction = false;

    // Check to see if the entry already exists
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Update the number of dirty entries if necessary
        if (isDirty && !pos->second->isDirty)
        {
            numDirtyEntries_++;
        }

        // Entry already exists, update it
        pos->second->data = value;
        pos->second->isDirty = isDirty;

        // Update the LRU data
        lruList_.erase(pos->second->lruRef);
        lruList_.push_front(key);
        pos->second->lruRef = lruList_.begin();
    }
    else
    {
        // If the cache is full, retrieve and perform the next eviction
        if (numEntries_ == maxEntries_)
        {
            KeyType lruKey = *(lruList_.rbegin());

            // Set the outbound parameters
            typename std::map<KeyType, EntryType*>::iterator evictee;
            evictee = keyEntryMap_.find(lruKey);
            assert(evictee != keyEntryMap_.end());
            outEvictedKey = evictee->first;
            outEvictedValue = evictee->second->data;
            outEvictedDirtyBit = evictee->second->isDirty;

            // Remove the entry
            this->remove(lruKey);

            // Indicate an eviction occurred
            hasEviction = true;
        }
        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->data = value;
        entry->isDirty = isDirty;

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
        numEntries_++;

        // Update the number of dirty entries if necessary
        if (isDirty)
        {
            numDirtyEntries_++;
        }
    }

    if (!hasEviction)
    {
        NoSuchEntry e;
        throw e;
    }
}

template<class KeyType, class ValueType>
void LRUCache<KeyType,ValueType>::remove(const KeyType& key)
{
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Cleanup the lru list
        lruList_.erase(pos->second->lruRef);

        // Update bookkeeping
        numEntries_--;
        if (getDirtyBit(key))
        {
            numDirtyEntries_--;
        }

        // Cleanup the EntryType memory
        delete pos->second;

        // Remove from the map
        keyEntryMap_.erase(pos);
    }
    else
    {
        NoSuchEntry e;
        throw e;
    }
}

template<class KeyType, class ValueType>
void LRUCache<KeyType,ValueType>::setDirtyBit(const KeyType& key, bool dirtyValue)
{
    typename std::map<KeyType, EntryType*>::iterator pos;

    // Search the map for key
    pos = keyEntryMap_.find(key);
    if (pos == keyEntryMap_.end())
    {
        NoSuchEntry e;
        throw e;
    }
    pos->second->isDirty = dirtyValue;
}

template<class KeyType, class ValueType>
bool LRUCache<KeyType,ValueType>::exists(const KeyType& key) const
{
    typename std::map<KeyType, EntryType*>::const_iterator pos;

    // Search the map for key
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        return true;
    }
    return false;
}

template<class KeyType, class ValueType>
ValueType LRUCache<KeyType,ValueType>::lookup(const KeyType& key)
{
    EntryType* entry = 0;
    typename std::map<KeyType, EntryType*>::iterator pos;

    // Search the map for key
    pos = keyEntryMap_.find(key);
    if (pos == keyEntryMap_.end())
    {
        NoSuchEntry e;
        throw e;
    }

    entry = pos->second;

    // Refresh the LRU list
    lruList_.erase(entry->lruRef);
    lruList_.push_front(key);
    entry->lruRef = lruList_.begin();
    return entry->data;
}

template<class KeyType, class ValueType>
bool LRUCache<KeyType,ValueType>::getDirtyBit(const KeyType& key) const
{
    // Search the map for key
    typename std::map<KeyType, EntryType*>::const_iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos == keyEntryMap_.end())
    {
        NoSuchEntry e;
        throw e;
     }

    return pos->second->isDirty;
}

template<class KeyType, class ValueType>
std::vector<KeyType> LRUCache<KeyType,ValueType>::getDirtyEntries() const
{
    std::vector<KeyType> dirtyEntries;

    // Loop thru the map to find all of the dirty entries
    typename MapType::const_iterator mapEnd = keyEntryMap_.end();
    typename MapType::const_iterator mapIter;
    for (mapIter = keyEntryMap_.begin(); mapIter != mapEnd; mapIter++)
    {
        if (mapIter->second->isDirty)
        {
            dirtyEntries.push_back(mapIter->first);
        }
    }

    return dirtyEntries;
}

template<class KeyType, class ValueType>
std::pair<KeyType, ValueType> LRUCache<KeyType,ValueType>::getLRU() const
{
    // Throw an exception if the cache is empty
    if (0 == numEntries_)
    {
        NoSuchEntry e;
        throw e;
    }

    // Locate the LRU entry
    assert(0 != lruList_.size());
    typename LRUListType::const_iterator lruIter = lruList_.end();
    typename MapType::const_iterator mapIter = keyEntryMap_.find(*(--lruIter));

    // Create a pair containing the LRU key and value
    KeyType lruKey = mapIter->first;
    ValueType lruValue = mapIter->second->data;
    return make_pair(lruKey, lruValue);
}

template<class KeyType, class ValueType>
std::size_t LRUCache<KeyType,ValueType>::capacity() const
{
    return maxEntries_;
}

template<class KeyType, class ValueType>
std::size_t LRUCache<KeyType,ValueType>::size() const
{
    assert(lruList_.size() == keyEntryMap_.size());
    assert(lruList_.size() == numEntries_);
    return numEntries_;
}

template<class KeyType, class ValueType>
double LRUCache<KeyType,ValueType>::percentDirty() const
{
    assert(numDirtyEntries_ <= maxEntries_);
    assert(numDirtyEntries_ <= numEntries_);
    //std::cerr << __FILE__ << ":" << __LINE__ << ":"
    //          << "Num Dirty: " << numDirtyEntries_ << " Max: " << maxEntries_ << endl;
    double percentDirty = double(numDirtyEntries_) / double(maxEntries_);
    return percentDirty;
}

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
