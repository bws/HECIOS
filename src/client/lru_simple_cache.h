#ifndef LRU_SIMPLE_CACHE_H
#define LRU_SIMPLE_CACHE_H

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
struct LRUSimpleCacheEntry
{
    ValueType extent;
    int address;
    double timeStamp;
    typename std::list<KeyType>::iterator lruRef;
};

/**
 *
 */
template <class KeyType, class ValueType>
class LRUSimpleCache
{
public:

    /** Convenience typedef of cache entries */
    typedef LRUSimpleCacheEntry<KeyType,ValueType> EntryType;

    /** Convencience typedef of the key-value map */
    typedef std::map<KeyType,EntryType*> MapType;

    /**
     * Constructor
     */
    LRUSimpleCache(int capacity);

    /** Destructor */
    ~LRUSimpleCache();

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
    int findOnlyKey(const KeyType& key);
    int findOnlyKeyValue(const KeyType& key, const ValueType& value);
    /**
     * @return the number of entries in the cache
     */
    int size() const;;
    
private:

    std::map<KeyType, EntryType*> keyEntryMap;
    std::list<KeyType> lruList;

    const int maxEntries_;
    //const double maxTime_;
    int numEntries_;
};

template <class KeyType, class ValueType>
LRUSimpleCache<KeyType,ValueType>::LRUSimpleCache(
    int capacity) :
    maxEntries_(capacity),
    numEntries_(0)
{
    assert(0 < maxEntries_);
}

template <class KeyType, class ValueType>
LRUSimpleCache<KeyType,ValueType>::~LRUSimpleCache()
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
void LRUSimpleCache<KeyType,ValueType>::insert(const KeyType& key,
                                                const ValueType& value)
{
    // Check to see if the entry already exists
    //printf("in the begin of insert\n");
    //fflush(stdout);
    int entrySize = (int) value;
    int prevPosEmpty, nextPosEmpty;
    prevPosEmpty = nextPosEmpty = 0;
    typename std::map<KeyType, EntryType*>::iterator prevPos;
    typename std::map<KeyType, EntryType*>::iterator nextPos;
    typename std::map<KeyType, EntryType*>::iterator pos;
    pos = keyEntryMap.find(key);
    if (pos != keyEntryMap.end())  // if entry already exists
    {
        // Entry already exists, update it
        pos->second->extent = value;
        pos->second->timeStamp = simulation.simTime();

        // Update the LRU data
        lruList.erase(pos->second->lruRef);
        lruList.push_front(key);
        pos->second->lruRef = lruList.begin();
    }
    else if(numEntries_ == 0) // if no entreis in the list
    {
        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->extent = value;
        entry->address = key;
        entry->timeStamp = simulation.simTime();

        // Add to the LRU list
        lruList.push_front(key);
        entry->lruRef = lruList.begin();

        // Insert the cache entry
        keyEntryMap.insert(std::make_pair(key, entry));


        
        // increment number of entries 
        numEntries_ = 1;
    }else if(numEntries_ == 1)
    {
        //printf("in numEntries == 1\n");
        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->extent = value;
        entry->address = key;
        entry->timeStamp = simulation.simTime();
        pos--;
        //printf("adding element with address %d and extent %d\n", 
        //        key, value);
        //printf("pos values are address %d and extent %d\n", 
        //        pos->second->address, pos->second->extent);

        // check if overlapping
        if(key < pos->second->address && 
                pos->second->address < entrySize + key)
        {   // if new entry overlaps current entry from right, new has smaller add
            // printf("got in here 1112\n");
            pos->second->extent = pos->second->extent +
                            pos->second->address - key;
            pos->second->address = key;
            // Add to the LRU list
            lruList.push_front(key);
            entry->lruRef = lruList.begin();

            // Insert the cache entry
            keyEntryMap.insert(std::make_pair(key, entry));


        }else if(pos->second->address < key &&
                    pos->second->address+pos->second->extent > key)
        {   // overlap from left into element, new is less
            // Add to the LRU list
            //printf("second position combining \n");
            pos->second->extent = value + key - pos->second->address;

        }else // just add entry if there are no overlaps
        {
            //printf("just adding \n");
            // Add to the LRU list
            lruList.push_front(key);
            entry->lruRef = lruList.begin();

            // Insert the cache entry
            keyEntryMap.insert(std::make_pair(key, entry));

            numEntries_++;
        }
    }else
    {
        // If the cache is full, evict an item according to LRU policy
        if (numEntries_ == maxEntries_)
        {
            KeyType key = *(lruList.rbegin());
            this->remove(key);
        }

        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->extent = value;
        entry->address = key;
        entry->timeStamp = simulation.simTime();

        //printf("got in here\n");
        
        // check if overlapping
        // if current entry overlaps next entries, while non-are overlapping
        if(entrySize+key > pos->second->address) // while
        {
            EntryType* entry = new EntryType();
            entry->extent = pos->second->extent+(pos->second->address-key);
            entry->timeStamp = simulation.simTime();
            if(numEntries_ > 0) lruList.erase(pos->second->lruRef);

            // Remove from the map
            //nextPos = pos+1;
            //pos--;
            if(numEntries_ > 1) 
            {
                keyEntryMap.erase(nextPos);
                numEntries_--;
            }
            
            //prevPos = pos;
            //pos++;
            
            // Cleanup the EntryType memory
            if(numEntries_ > 1) delete prevPos->second;
            
            
        }

        // if previous entry overlaps current entry
        //printf("more towars end of adding to list\n");
        //fflush(stdout);
        if((keyEntryMap.end()--)->second->extent != pos->second->extent)
        {
            prevPos = pos--;
            pos++;
        }else
        {
            prevPosEmpty = 1;;

        }


        if(keyEntryMap.begin()->second->extent != pos->second->extent)
        {     
            nextPos= pos++;
            pos--;
        }else
        {
            nextPosEmpty = 1;
        }
        
        if(!prevPosEmpty &&
            prevPos->second->extent+prevPos->second->address > key)
        {

            EntryType* entry = new EntryType();
            entry->extent = prevPos->second->extent+
                ((entrySize+key) - 
                (prevPos->second->extent+prevPos->second->address));
            entry->timeStamp = simulation.simTime();
            lruList.erase(prevPos->second->lruRef);


            // Remove from the map
            keyEntryMap.erase(prevPos);
            numEntries_--;

            // Cleanup the EntryType memory
            delete prevPos->second;

        }

        // Add to the LRU list
        lruList.push_front(key);
        entry->lruRef = lruList.begin();

        // Insert the cache entry
        keyEntryMap.insert(std::make_pair(key, entry));
        numEntries_++;
    }
    printf("end of function \n");
    fflush(stdout);
}

template<class KeyType, class ValueType>
void LRUSimpleCache<KeyType,ValueType>::remove(const KeyType& key)
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
    
    if(pos != keyEntryMap.begin())pos--;
    if(pos->second->address <= key &&
        pos->second->address+pos->second->extent >= key)
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



// return 0 if not found in cache, 1 if found
template<class KeyType, class ValueType>
int LRUSimpleCache<KeyType,ValueType>::findOnlyKey(const KeyType& key)
{
    int toReturn = 0;
    if(lookup(key) != 0) toReturn = 1;    
    return toReturn;
}


// return - if not found in cache, 1 if found
template<class KeyType, class ValueType>
int  LRUSimpleCache<KeyType,ValueType>::findOnlyKeyValue(const KeyType& key, const ValueType& value)
{
    int toReturn = 0;
    EntryType* foundEntry = lookup(key);
    if(foundEntry != 0 && 
    ((foundEntry->address+foundEntry->extent) < (key+value)))
    {
        toReturn = 1;

    }
    return toReturn;
}



template<class KeyType, class ValueType>
typename LRUSimpleCache<KeyType,ValueType>::EntryType*
LRUSimpleCache<KeyType,ValueType>::lookup(const KeyType& key)
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
    if(pos != keyEntryMap.begin())pos--;
    
    if(pos->second->address <= key &&
        pos->second->address+pos->second->extent >= key)
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
int LRUSimpleCache<KeyType,ValueType>::size() const
{
    assert(lruList.size() == keyEntryMap.size());
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
