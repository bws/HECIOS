#ifndef LRU_CACHE_H
#define LRU_CACHE_H

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

    std::map<KeyType, EntryType*> keyEntryMap;
    std::list<KeyType> lruList;

    const int maxEntries_;
    const double maxTime_;
    int numEntries_;
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
    for (iter = keyEntryMap.begin(); iter != keyEntryMap.end(); ++iter)
    {
        //std::cerr << "Want to delete" << iter->first << endl;
        delete iter->second;
    }

    // Clear containers
    lruList.clear();
    keyEntryMap.clear();
}


template<class KeyType, class ValueType>
void LRUTimeoutCache<KeyType,ValueType>::insert(const KeyType& key,
                                                const ValueType& value)
{
    // Check to see if the entry already exists
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap.find(key);
    if (pos != keyEntryMap.end())
    {
        // Entry already exists, update it
        pos->second->data = value;
        pos->second->timeStamp = simulation.simTime();

        // Update the LRU data
        lruList.erase(pos->second->lruRef);
        lruList.push_front(key);
        pos->second->lruRef = lruList.begin();
    }
    else
    {
        // If the cache is full, evict an item according to LRU policy
        if (numEntries_ == maxEntries_)
        {
            KeyType key = *(lruList.rbegin());
            this->remove(key);
        }

        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->data = value;
        entry->timeStamp = simulation.simTime();

        // Add to the LRU list
        lruList.push_front(key);
        entry->lruRef = lruList.begin();

        // Insert the cache entry
        keyEntryMap.insert(std::make_pair(key, entry));
        numEntries_++;
    }
}

template<class KeyType, class ValueType>
void LRUTimeoutCache<KeyType,ValueType>::remove(const KeyType& key)
{
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap.find(key);
    if (pos != keyEntryMap.end())
    {
        // Cleanup the lru list
        lruList.erase(pos->second->lruRef);

        // Cleanup the EntryType memory
        delete pos->second;

        // Remove from the map
        keyEntryMap.erase(pos);
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
    pos = keyEntryMap.find(key);
    if (pos != keyEntryMap.end())
    {
        entry = pos->second;

        // Refresh the LRU list
        lruList.erase(entry->lruRef);
        lruList.push_front(key);
        entry->lruRef = lruList.begin();
    }
    return entry;
}

template<class KeyType, class ValueType>
int LRUTimeoutCache<KeyType,ValueType>::size() const
{
    if (lruList.size() != keyEntryMap.size())
    {
        std::cerr << "ERROR Size mismatch: lruList ->" << lruList.size()
                  << " map-->" << keyEntryMap.size() << endl;
    }
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
