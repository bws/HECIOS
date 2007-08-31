#ifndef LRU_TIMEOUT_CACHE_H
#define LRU_TIMEOUT_CACHE_H
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
#include <iostream>
#include <list>
#include <map>
#include <utility>
#include <omnetpp.h>

/**
 * A CacheEntry wrapper that includes a simulation timestamp
 */
template <class KeyType, class ValueType>
struct LRUTimeoutCacheEntry
{
    ValueType data;
    double timeStamp;
    typename std::list<KeyType>::iterator lruRef;
};

/**
 *
 */
template <class KeyType, class ValueType>
class LRUTimeoutCache
{
public:

    /** Convenience typedef of cache entries */
    typedef LRUTimeoutCacheEntry<KeyType,ValueType> EntryType;

    /** Convencience typedef of the key-value map */
    typedef std::map<KeyType,EntryType*> MapType;

    /**
     * Constructor
     */
    LRUTimeoutCache(int capacity, double timeOut);

    /** Destructor */
    ~LRUTimeoutCache();

    /**
     * Insert a Key-Value pair into the cache.  If the cache already
     * contains more entries than the maximum size, evict the least
     * recently used item
     */
    void insert(const KeyType& key, const ValueType& value);

    /** Remove the value for key from the cache */
    void remove(const KeyType& key);

    /**
     * @return The EntryType value wrapper for key.  The wrapper allows
     * the user to also determine the last time the entry was accessed.
     * If no entry exists for key, return 0
     */
    EntryType* lookup(const KeyType& key);

    /**
     * @return the number of entries in the cache
     */
    int size() const;;
    
private:

    std::map<KeyType, EntryType*> keyEntryMap_;
    std::list<KeyType> lruList_;

    const size_t maxEntries_;
    const double maxTime_;
    size_t numEntries_;
};

template <class KeyType, class ValueType>
LRUTimeoutCache<KeyType,ValueType>::LRUTimeoutCache(
    int capacity, double timeOut) :
    maxEntries_(capacity),
    maxTime_(timeOut),
    numEntries_(0)
{
    assert(0 < maxEntries_);
    assert(0.0 < maxTime_);
}

template <class KeyType, class ValueType>
LRUTimeoutCache<KeyType,ValueType>::~LRUTimeoutCache()
{
    // Delete any EntryTypes still contained in the map
    typename std::map<KeyType, EntryType*>::iterator iter;
    for (iter = keyEntryMap_.begin(); iter != keyEntryMap_.end(); ++iter)
    {
        //std::cerr << "Want to delete" << iter->first << endl;
        delete iter->second;
    }

    // Clear containers
    lruList_.clear();
    keyEntryMap_.clear();
}


template<class KeyType, class ValueType>
void LRUTimeoutCache<KeyType,ValueType>::insert(const KeyType& key,
                                                const ValueType& value)
{
    // Check to see if the entry already exists
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Entry already exists, update it
        pos->second->data = value;
        pos->second->timeStamp = simulation.simTime();

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
        entry->timeStamp = simulation.simTime();

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
        numEntries_++;
    }
}

template<class KeyType, class ValueType>
void LRUTimeoutCache<KeyType,ValueType>::remove(const KeyType& key)
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
typename LRUTimeoutCache<KeyType,ValueType>::EntryType*
LRUTimeoutCache<KeyType,ValueType>::lookup(const KeyType& key)
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
    return entry;
}

template<class KeyType, class ValueType>
int LRUTimeoutCache<KeyType,ValueType>::size() const
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
