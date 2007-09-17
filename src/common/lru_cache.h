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
     */
    void insert(const KeyType& key, const ValueType& value);

    /** Remove the value for key from the cache */
    void remove(const KeyType& key);

    /**
     * Check if an entry exists without modifying its LRU status
     *
     * @return true if a value exists for the given key
     */
    bool exists(const KeyType& key) const;
    
    /**
     * @return The EntryType value wrapper for key.  The wrapper allows
     * the user to also determine the last time the entry was accessed.
     * If no entry exists for key, return 0
     *
     * @throw NoSuchEntry if key does not exist
     */
    ValueType lookup(const KeyType& key);

    /**
     * @return the next entry that will be evicted on a new insertion
     *
     * @throw NoSuchEntry if the cache is empty
     */
    ValueType nextEviction() const;

    /**
     * @return the cache capacity
     */
    int capacity() const;
    
    /**
     * @return the number of entries in the cache
     */
    int size() const;

private:

    std::map<KeyType, EntryType*> keyEntryMap_;
    std::list<KeyType> lruList_;

    const std::size_t maxEntries_;
    std::size_t numEntries_;
};

template <class KeyType, class ValueType>
LRUCache<KeyType,ValueType>::LRUCache(int capacity)
    : maxEntries_(capacity),
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
                                         const ValueType& value)
{
    // Check to see if the entry already exists
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Entry already exists, update it
        pos->second->data = value;

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
            KeyType key = *(lruList_.rbegin());
            this->remove(key);
        }

        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->data = value;

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
        numEntries_++;
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

        // Cleanup the EntryType memory
        delete pos->second;

        // Remove from the map
        keyEntryMap_.erase(pos);
        numEntries_--;
    }
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
    if (pos != keyEntryMap_.end())
    {
        entry = pos->second;

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
    return entry->data;
}

template<class KeyType, class ValueType>
ValueType LRUCache<KeyType,ValueType>::nextEviction() const
{
    EntryType* entry = 0;
        
    if (0 != numEntries_)
    {
        assert(0 != lruList_.size());
        typename LRUListType::const_iterator lruIter = lruList_.end();
        typename MapType::const_iterator mapIter =
            keyEntryMap_.find(*(--lruIter));
        assert(keyEntryMap_.end() != mapIter);
        entry = mapIter->second;
    }
    else
    {
        NoSuchEntry e;
        throw e;
    }
    return entry->data;
}

template<class KeyType, class ValueType>
int LRUCache<KeyType,ValueType>::capacity() const
{
    return maxEntries_;
}

template<class KeyType, class ValueType>
int LRUCache<KeyType,ValueType>::size() const
{
    assert(lruList_.size() == keyEntryMap_.size());
    assert(lruList_.size() == numEntries_);
    return numEntries_;
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
