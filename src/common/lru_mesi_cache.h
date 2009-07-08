#ifndef LRU_MESI_CACHE_H
#define LRU_MESI_CACHE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include <functional>
#include <list>
#include <map>
#include <utility>
#include <vector>
#include "spfs_exceptions.h"

/**
 * A helper class for filtering LRUCache entries
 */

/**
 * A simple fixed size cache using LRU replacement.
 */
template <class KeyType, class ValueType>
class LRUMesiCache
{
public:

    enum State {NULL_STATE = 0, MODIFIED, EXCLUSIVE, SHARED, INVALID};
    /**
     * A cache entry for
     */
    struct EntryType
    {
        ValueType data;
        State state;
        typename std::list<KeyType>::iterator lruRef;
    };

    /**
     * Function object that filter cache entries based on the key
     */
    struct FilterFunctor : std::binary_function<KeyType, EntryType, bool>
    {
        bool operator()(const KeyType& key, const EntryType& entry) const
        {
            return filter(key, entry.data, entry.state);
        }

        virtual bool filter(const KeyType& key, const ValueType& val, State state) const = 0;
    };

    /** Convenience typedef of the key-value map */
    typedef std::map<KeyType, EntryType*> MapType;

    /** Convenience typedef of the lru list */
    typedef std::list<KeyType> LRUListType;

    /**
     * Constructor
     */
    LRUMesiCache(int capacity);

    /** Destructor */
    ~LRUMesiCache();

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
                State state = SHARED);

    /**
     * Perform the cache insertion and set the evicted value and dirty bit
     * in the outbound parameters.
     *
     * @throw NoSuchEntry if no eviction occurs
     */
    void insertAndRecall(const KeyType& key,
                         const ValueType& value,
                         State state,
                         KeyType& outEvictedKey,
                         ValueType& outEvictedValue,
                         State& outState);

    /**
     * Remove the value for key from the cache
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    void remove(const KeyType& key);

    /**
     * Set the state-bit for the cache key in question without updating
     * the LRU status
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    void setState(const KeyType& key, State state);

    /**
     * @return The value for key and update the LRU ordering
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    ValueType lookup(const KeyType& key);

    /**
     * @return The value for key and update the LRU ordering
     *
     * @throw NoSuchEntry if no entry exists for the key
     */
    State lookupState(const KeyType& key);

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
    State getState(const KeyType& key) const;

    /**
     * @return a vector of all the modified cache values.  Does not update
     *         the LRU status
     */
    std::vector<KeyType> getModifiedEntries() const;

    /**
     * @return a vector of all the cache values that the filter returns
     *   true for.
     */
    std::set<ValueType> getFilteredEntries(const FilterFunctor& filter) const;

    /**
     * @return a vector of all the cache keys that the filter returns
     *   true for.
     */
    std::set<KeyType> getFilteredKeys(const FilterFunctor& filter) const;

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

    /** @return the percentage of the cache capacity that is modified */
    double percentModified() const;

private:

    std::map<KeyType, EntryType*> keyEntryMap_;
    std::list<KeyType> lruList_;

    const std::size_t maxEntries_;
    std::size_t numModifiedEntries_;
    std::size_t numEntries_;
};

template <class KeyType, class ValueType>
LRUMesiCache<KeyType,ValueType>::LRUMesiCache(int capacity)
    : maxEntries_(capacity),
      numModifiedEntries_(0),
      numEntries_(0)
{
    assert(0 < maxEntries_);
}

template <class KeyType, class ValueType>
LRUMesiCache<KeyType,ValueType>::~LRUMesiCache()
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
void LRUMesiCache<KeyType,ValueType>::insert(const KeyType& key,
                                             const ValueType& value,
                                             State state)
{
    // Check to see if the entry already exists
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Update the number of dirty entries if necessary
        if (MODIFIED == state &&
            MODIFIED != pos->second->state)
        {
            numModifiedEntries_++;
        }

        // Entry already exists, update it
        pos->second->data = value;
        pos->second->state = state;

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
        entry->state = state;

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
        numEntries_++;

        // Update the number of dirty entries if necessary
        if (MODIFIED == state)
        {
            numModifiedEntries_++;
        }
    }
}

template<class KeyType, class ValueType>
void LRUMesiCache<KeyType,ValueType>::insertAndRecall(const KeyType& key,
                                                  const ValueType& value,
                                                  State state,
                                                  KeyType& outEvictedKey,
                                                  ValueType& outEvictedValue,
                                                  State& outEvictedState)
{
    bool hasEviction = false;

    // Check to see if the entry already exists
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Update the number of dirty entries if necessary
        if (MODIFIED == state &&
            MODIFIED != pos->second->state)
        {
            numModifiedEntries_++;
        }

        // Entry already exists, update it
        pos->second->data = value;
        pos->second->state = state;

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
            outEvictedState = evictee->second->state;

            // Remove the entry
            this->remove(lruKey);

            // Indicate an eviction occurred
            hasEviction = true;
        }
        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->data = value;
        entry->state = state;

        // Add to the LRU list
        lruList_.push_front(key);
        entry->lruRef = lruList_.begin();

        // Insert the cache entry
        keyEntryMap_.insert(std::make_pair(key, entry));
        numEntries_++;

        // Update the number of dirty entries if necessary
        if (MODIFIED == state)
        {
            numModifiedEntries_++;
        }
    }

    if (!hasEviction)
    {
        NoSuchEntry e;
        throw e;
    }
}

template<class KeyType, class ValueType>
void LRUMesiCache<KeyType,ValueType>::remove(const KeyType& key)
{
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos != keyEntryMap_.end())
    {
        // Cleanup the lru list
        lruList_.erase(pos->second->lruRef);

        // Update bookkeeping
        numEntries_--;
        if (MODIFIED == getState(key))
        {
            numModifiedEntries_--;
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
void LRUMesiCache<KeyType,ValueType>::setState(const KeyType& key, State stateValue)
{
    typename std::map<KeyType, EntryType*>::iterator pos;

    // Search the map for key
    pos = keyEntryMap_.find(key);
    if (pos == keyEntryMap_.end())
    {
        NoSuchEntry e;
        throw e;
    }

    // Adjust the number of modified entries if needed
    if (MODIFIED == pos->second->state)
    {
        numModifiedEntries_--;
    }
    if (MODIFIED == stateValue)
    {
        numModifiedEntries_++;
    }

    pos->second->state = stateValue;
}

template<class KeyType, class ValueType>
bool LRUMesiCache<KeyType,ValueType>::exists(const KeyType& key) const
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
ValueType LRUMesiCache<KeyType,ValueType>::lookup(const KeyType& key)
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
typename LRUMesiCache<KeyType,ValueType>::State
LRUMesiCache<KeyType,ValueType>::lookupState(const KeyType& key)
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
    return entry->state;
}

template<class KeyType, class ValueType>
typename LRUMesiCache<KeyType,ValueType>::State
LRUMesiCache<KeyType,ValueType>::getState(const KeyType& key) const
{
    // Search the map for key
    typename std::map<KeyType, EntryType*>::const_iterator pos;
    pos = keyEntryMap_.find(key);
    if (pos == keyEntryMap_.end())
    {
        NoSuchEntry e;
        throw e;
     }

    return pos->second->state;
}

template<class KeyType, class ValueType>
std::vector<KeyType> LRUMesiCache<KeyType,ValueType>::getModifiedEntries() const
{
    std::vector<KeyType> dirtyEntries;

    // Loop thru the map to find all of the dirty entries
    typename MapType::const_iterator mapEnd = keyEntryMap_.end();
    typename MapType::const_iterator mapIter;
    for (mapIter = keyEntryMap_.begin(); mapIter != mapEnd; mapIter++)
    {
        if (MODIFIED == mapIter->second->state)
        {
            dirtyEntries.push_back(mapIter->first);
        }
    }

    return dirtyEntries;
}

template<class KeyType, class ValueType>
std::set<ValueType> LRUMesiCache<KeyType,ValueType>::getFilteredEntries(
    const LRUMesiCache<KeyType,ValueType>::FilterFunctor& filterFunc) const
{
    std::set<ValueType> filteredEntries;

    // Loop thru the map to find all of the dirty entries
    typename MapType::const_iterator mapEnd = keyEntryMap_.end();
    typename MapType::const_iterator mapIter;
    for (mapIter = keyEntryMap_.begin(); mapIter != mapEnd; mapIter++)
    {
        if (filterFunc(mapIter->first, *mapIter->second))
        {
            filteredEntries.insert(mapIter->second->data);
        }
    }

    return filteredEntries;
}

template<class KeyType, class ValueType>
std::set<KeyType> LRUMesiCache<KeyType,ValueType>::getFilteredKeys(
    const LRUMesiCache<KeyType,ValueType>::FilterFunctor& filterFunc) const
{
    std::set<KeyType> filteredEntries;

    // Loop thru the map to find all of the dirty entries
    typename MapType::const_iterator mapEnd = keyEntryMap_.end();
    typename MapType::const_iterator mapIter;
    for (mapIter = keyEntryMap_.begin(); mapIter != mapEnd; mapIter++)
    {
        if (filterFunc(mapIter->first, *mapIter->second))
        {
            filteredEntries.insert(mapIter->first);
        }
    }

    return filteredEntries;
}

template<class KeyType, class ValueType>
std::pair<KeyType, ValueType> LRUMesiCache<KeyType,ValueType>::getLRU() const
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
std::size_t LRUMesiCache<KeyType,ValueType>::capacity() const
{
    return maxEntries_;
}

template<class KeyType, class ValueType>
std::size_t LRUMesiCache<KeyType,ValueType>::size() const
{
    assert(lruList_.size() == keyEntryMap_.size());
    assert(lruList_.size() == numEntries_);
    return numEntries_;
}

template<class KeyType, class ValueType>
double LRUMesiCache<KeyType,ValueType>::percentModified() const
{
    assert(numModifiedEntries_ <= maxEntries_);
    assert(numModifiedEntries_ <= numEntries_);
    //std::cerr << __FILE__ << ":" << __LINE__ << ":"
    //          << "Num Dirty: " << numDirtyEntries_ << " Max: " << maxEntries_ << endl;
    double percentModified = double(numModifiedEntries_) / double(maxEntries_);
    return percentModified;
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
