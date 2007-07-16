#ifndef LRU_SIMPLE_CACHE_H
#define LRU_SIMPLE_CACHE_H

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <utility>
#include <omnetpp.h>
#include "lru_replace_policy.h"

using namespace std;

/**
 * A CacheEntry wrapper that includes a simulation timestamp
 */
//template <class KeyType, class ValueType>
/*struct LRUSimpleCacheEntry
{
    int extent;
    int address;
    double timeStamp;
    std::list<int>::iterator lruRef;
};*/

/**
 *
 */
//template <class KeyType, class ValueType>
class LRUSimpleCache
{
public:

    /** Convenience typedef of cache entries */
    typedef LRUSimpleCacheEntry EntryType;

    /** Convencience typedef of the key-value map */
    typedef map<int,EntryType*> MapType;

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
    void insert(const int& key, const int& value);

    /** Remove the value for key from the cache */
    void remove(const int& key);

    /**
     * @return The EntryType value wrapper for key.  The wrapper allows
     * the user to also determine the last time the entry was accessed.
     * If no entry exists for key, return 0
     */
    EntryType* lookup(const int& key);
    int findOnlyKey(const int& key);
    int findOnlyKeyValue(const int& key, const int& value);
    int findOnlyKeyValueOffset(const int& key, const int& value, const int offset);
    int returnEvict();
    /**
     * @return the number of entries in the cache
     */
    int size() const;;
    int physSize();
    
private:

    map<int, EntryType*> keyEntryMap;
    list<int> lruList;

    const int maxEntries_;
    const int maxPhysSize_;
    //const double maxTime_;
    int numEntries_;
    int currentPhysSize_;
    LruReplacePolicy lruPolicy; //-----_-_-___-_-_-here -_-_-_-_//
    int useFIFO;
    int blockSize;
};

//inline LRUSimpleCache

//template <class KeyType, class ValueType>
inline LRUSimpleCache::LRUSimpleCache(int capacity) :
    maxEntries_(capacity),
    maxPhysSize_(capacity*8000),
    numEntries_(0),
    currentPhysSize_(0)
{
    assert(0 < maxEntries_);
    assert(0 < maxPhysSize_);
    useFIFO = 0;
    blockSize = 1; // size of each block, i.e. 4,16,32,4k...
}

//template <class KeyType, class ValueType>
inline LRUSimpleCache::~LRUSimpleCache()
{
    // Delete any EntryTypes still contained in the map
    std::map<int, EntryType*>::iterator iter;
    for (iter = keyEntryMap.begin(); iter != keyEntryMap.end(); ++iter)
    {
        //std::cerr << "Want to delete" << iter->first << endl;
        delete iter->second;
    }

    // Clear containers
    lruList.clear();
    keyEntryMap.clear();
}

// two arg. insert w/o offset, defualts to offset of zero
/*inline void LRUSimpleCache::insert(const int& key,                                                const int& value)
{
    insert(key, value, 0);
}*/

//template<class KeyType, class ValueType>
inline void LRUSimpleCache::insert(const int& key,                                                const int& value) //, const int& inOffset)
{
    // calculate correct block start and end for current block
    //int keyBlock = (int) floor(key/blockSize) * blockSize;
    //int valueBlock = (int) ceil(value/blockSize) * blockSize;
    //printf("key is %d, value is %d\n", keyBlock, valueBlock);
    
    // Check to see if the entry already exists
    int entrySize = (int) value;
    int prevPosEmpty, nextPosEmpty;
    prevPosEmpty = nextPosEmpty = 0;
    std::map<int, EntryType*>::iterator prevPos;
    std::map<int, EntryType*>::iterator nextPos;
    std::map<int, EntryType*>::iterator pos;
    pos = keyEntryMap.find(key);
    if (pos != keyEntryMap.end())  // if entry already exists
    {
        printf("found entry, merging\n");
        // Entry already exists, update it
        if(value > pos->second->extent)
        {
            currentPhysSize_ -= pos->second->extent;
            currentPhysSize_ += value;
            pos->second->extent = value;
            //pos->second->offset = inOffset;
        }
        pos->second->timeStamp = simulation.simTime();

        // Update the LRU data
        /*lruList.erase(pos->second->lruRef);
        if(useFIFO == 1)
        {
            lruList.push_back(key);
        }
        else
        {
            lruList.push_front(key);
        }
        pos->second->lruRef = lruList.begin();*/
        pos->second->lruRef = lruPolicy.PolicyUpdate(&lruList,
                            pos->second->lruRef, key);
    }
    else if(numEntries_ == 0) // if no entreis in the list
    {
        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->extent = value;
        entry->address = key;
        entry->timeStamp = simulation.simTime();
        currentPhysSize_ = value;
        // Add to the LRU list
        /*if(useFIFO)
        {
            lruList.push_back(key);
        }
        else
        {
            lruList.push_front(key);
        }
        entry->lruRef = lruList.begin();*/
        entry->lruRef = lruPolicy.PolicyInsert(&lruList, key);

        // Insert the cache entry
        keyEntryMap.insert(std::make_pair(key, entry));

        // printf("inserting here %d %d %d %d \n", entry->address, key,
        //                 entry->extent, value);


        // increment number of entries 
        numEntries_ = 1;
    }else if(numEntries_ == 1)
    {
        //printf("in numEntries == 1\n");
        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->extent = value;
        currentPhysSize_ += value;
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
            /*if(useFIFO)
            {
                lruList.push_back(key);
            }
            else
            {
                lruList.push_front(key);
            }
            entry->lruRef = lruList.begin();*/
            entry->lruRef = lruPolicy.PolicyInsert(&lruList, key);

            // Insert the cache entry
            keyEntryMap.insert(std::make_pair(key, entry));


        }else if(pos->second->address < key &&
                    pos->second->address+pos->second->extent > key)
        {   // overlap from left into element, new is less
            // Add to the LRU list
            //printf("second position combining \n");
            currentPhysSize_ -= pos->second->extent;
            pos->second->extent = value + key - pos->second->address;    
            //printf("got in here yayayaya \n");
            currentPhysSize_ += pos->second->extent;
            //entry->lruRef = lruList.begin();

        }else // just add entry if there are no overlaps
        {
            //printf("just adding \n");
            // Add to the LRU list
            /*if(useFIFO)
            {
                lruList.push_back(key);
            }
            else
            {
                lruList.push_front(key);
            }
            entry->lruRef = lruList.begin();*/
            entry->lruRef = lruPolicy.PolicyInsert(&lruList, key);

            // Insert the cache entry
            keyEntryMap.insert(std::make_pair(key, entry));
            currentPhysSize_+= entry->extent;
            numEntries_++;
        }
    }else
    {
        // If the cache is full, evict an item according to LRU policy
        while ((numEntries_ == maxEntries_ || 
            (currentPhysSize_ + value) > maxPhysSize_)
            && value < maxPhysSize_)
            //&& numEntries > 0)
        {
            //int key = *(lruList.rbegin());
            //this->remove(key);
            int toEvict = lruPolicy.GetEvictIndex(&lruList);
            printf("Evicting entries, block cache full %d %d %d\n", value,
                                currentPhysSize_,
                                numEntries_);
            fflush(stdout);
            this->remove(toEvict);
        }

        // Fill out the Cache entry data
        EntryType* entry = new EntryType();
        entry->extent = value;
        currentPhysSize_ += value;
        entry->address = key;
        entry->timeStamp = simulation.simTime();

        //printf("got in here\n");
        
        // check if overlapping
        // if current entry overlaps next entries, while non-are overlapping
        if(entrySize+key > pos->second->address && pos != keyEntryMap.end())
        {
            EntryType* entry = new EntryType();
            currentPhysSize_ -= pos->second->extent;
            entry->extent = pos->second->extent+(pos->second->address-key);
            entry->timeStamp = simulation.simTime();
            //if(numEntries_ == 0) 
            lruList.erase(pos->second->lruRef);
            currentPhysSize_ += pos->second->extent;

            // Remove from the map
            //nextPos = pos+1;
            //pos--;
            keyEntryMap.erase(nextPos);   
            numEntries_--;
            /*if(numEntries_ > 1) 
            {
                keyEntryMap.erase(nextPos);   
                numEntries_--;
            }*/
            

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
            currentPhysSize_ -= pos->second->extent;
            entry->extent = prevPos->second->extent+
                ((entrySize+key) - 
                (prevPos->second->extent+prevPos->second->address));
            entry->timeStamp = simulation.simTime();
            currentPhysSize_ += pos->second->extent;
            lruList.erase(prevPos->second->lruRef);


            // Remove from the map
            keyEntryMap.erase(prevPos);
            numEntries_--;

            // Cleanup the EntryType memory
            delete prevPos->second;

        }

        // Add to the LRU list
        /*if(useFIFO)
        {
            lruList.push_back(key);
        }
        else
        {
            lruList.push_front(key);
        }
        entry->lruRef = lruList.begin();*/
        entry->lruRef = lruPolicy.PolicyInsert(&lruList, key);

        // Insert the cache entry
        keyEntryMap.insert(std::make_pair(key, entry));
        numEntries_++;
    }
    //printf("end of function %d\n", numEntries_);
    fflush(stdout);
}

//template<class KeyType, class ValueType>
inline void LRUSimpleCache::remove(const int& key)
{
    if(numEntries_ > 0)
    {
    //printf("printing entire lru list\n");
    list<int>::iterator printIter;
    int removedItem = 0;
    /*for(printIter = lruList.begin(); printIter != lruList.end(); printIter++)
    {
        printf("value is %d \n", *printIter);

    }*/
    std::map<int, EntryType*>::iterator pos;
    pos = keyEntryMap.find(key);
    //printf("in here ---\n");
    printf("extent to remove is %d key %d %d %d\n", pos->second->extent, key,
                pos->second->address, pos->second->address+pos->second->extent);
    if (pos != keyEntryMap.end() || pos->second->address == key)
    {
        printf("in the removal\n");
        currentPhysSize_ -= pos->second->extent; // update size then remove item
        // Cleanup the lru list
        lruList.remove(pos->second->address);
        //lruList.erase(pos->second->lruRef);
        

        // Remove from the map
        keyEntryMap.erase(pos->first);
        numEntries_--;
        removedItem = 1;
        //lruList.resize(numEntries_);
        
        // Cleanup the EntryType memory
        //delete &(pos->second);
    }
    
    if(pos != keyEntryMap.begin())pos--;
    //printf("in here ---\n");
    printf("extent to remove is %d key %d %d %d\n", pos->second->extent, key,
                pos->second->address, pos->second->address+pos->second->extent);
    if(pos->second->address <= key &&
        pos->second->address+pos->second->extent >= key
        && numEntries_>0
        && !removedItem) // if the given entry starts inside another block, 
                          // remove the entire block
    {
        printf("in the block removal part 2\n");
        currentPhysSize_ -= pos->second->extent; // update size then remove
        // Cleanup the lru list
        //if (pos != keyEntryMap.end())
        lruList.erase(pos->second->lruRef);

        // Remove from the map
        //keyEntryMap.erase(pos);
        keyEntryMap.erase(pos->first);
        numEntries_--;
        // Cleanup the EntryType memory
        //delete pos->second;
    }
    }
}



// return 0 if not found in cache, 1 if found
//template<class KeyType, class ValueType>
inline int LRUSimpleCache::findOnlyKey(const int& key)
{
    int toReturn = 0;
    if(lookup(key) != 0) toReturn = 1;    
    return toReturn;
}


// return - if not found in cache, 1 if found
//template<class KeyType, class ValueType>
inline int  LRUSimpleCache::findOnlyKeyValue(const int& key, const int& value)
{
    int toReturn = 0;
    EntryType* foundEntry = lookup(key);
   // if(foundEntry == 0) printf("well damen it\n");
   // printf("ad+ex= %d, key+val=%d\n", 
   // (foundEntry->address+foundEntry->extent),(key+value));
    if(foundEntry != 0 && 
    ((foundEntry->address+foundEntry->extent) > (key+value)))
    {
        toReturn = 1;

    }
    return toReturn;
}

// also looks for offsets
inline int  LRUSimpleCache::findOnlyKeyValueOffset
                (const int& key, const int& value, const int offset)
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


//template<class KeyType, class ValueType>
inline LRUSimpleCache::EntryType*
LRUSimpleCache::lookup(const int& key)
{
    EntryType* entry = 0;
    std::map<int, EntryType*>::iterator pos;
    if(numEntries_ == 0)
        return entry;
    
    // Search the map for key
    pos = keyEntryMap.find(key);
    if (pos != keyEntryMap.end())
    {
        entry = pos->second;

        // Refresh the LRU list
        /*lruList.erase(entry->lruRef);
        if(useFIFO)
        {
            lruList.push_back(key);
        }
        else
        {
            lruList.push_front(key);
        }
        entry->lruRef = lruList.begin();*/
        entry->lruRef = lruPolicy.PolicyUpdate(&lruList,
                            entry->lruRef, key);
    }
    if(pos != keyEntryMap.begin())pos--;
    
    if(pos->second->address <= key &&
        pos->second->address+pos->second->extent >= key)
    {
         entry = pos->second;
        // Refresh the LRU list
        /*lruList.erase(entry->lruRef);
        if(useFIFO)
        {
            lruList.push_back(key);
        }
        else
        {
            lruList.push_front(key);
        }
        entry->lruRef = lruList.begin();*/
        entry->lruRef = lruPolicy.PolicyUpdate(&lruList,
                            entry->lruRef, key);
    }
    return entry;
}

inline int LRUSimpleCache::returnEvict()
{
    int toEvict = lruPolicy.GetEvictIndex(&lruList);
    //int toEvict = (*(lruList.rbegin()));
    printf("to evict is %d\n", toEvict);
    return toEvict;
}

inline int LRUSimpleCache::physSize()
{
    return currentPhysSize_;
}

//template<class KeyType, class ValueType>
inline int LRUSimpleCache::size() const
{
    //assert(lruList.size() == keyEntryMap.size());
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
