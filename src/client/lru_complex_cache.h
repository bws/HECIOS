// Name: Michael Bassily
// File: lru_complex_cache.h

#ifndef LRU_COMPLEX_CACHE_H
#define LRU_COMPLEX_CACHE_H

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <utility>
#include <omnetpp.h>
#include "lru_simple_cache.h"
#include "lru_replace_policy.h"

using namespace std;

/**
 * A CacheEntry wrapper that includes a simulation timestamp
 */
struct LRUComplexCacheEntry
{
    int handle;
    LRUSimpleCache *blockCache;
    std::list<int>::iterator blockLruRef;
};

class LRUComplexCache
{
public:

    /*** convenient typedef of cache entries */
    typedef LRUComplexCacheEntry ComplexEntryType;    

    /**
    * Constructor
    */
    LRUComplexCache(long int capacity, long int size);

    /** Destructor */
    ~LRUComplexCache();

    /**
    * Insert a handle, offset, extent set  into the cache.  If the cache already
    * contains more entries than the maximum size, evict the least
    * recently used item.
    */
    void insert(const int& handle, const int& extent, const int& offset);
    /** Remove the value for key from the cache */
    void removeHandle(const int& handle);
    void removeOffset(const int& handle, const int& offset);

    /**
     * @return The EntryType value wrapper for key.  The wrapper allows
     * the user to also determine the last time the entry was accessed.
     * If no entry exists for key, return 0
     */
    ComplexEntryType* lookup(const int& handle, const int& offset, 
                             int extent);
    int findOnlyHandle(const int& handle);
    int findOnlyHandleOffset(const int& handle, const int& offset);
    int findOnlyHandleOffsetExtent(const int& handle, const int&offset,
                                    const int& extent);
    int size() const;
    int physSize();
    void mapPrint();

private:
    map<int, ComplexEntryType*> fileEntryMap;
    list<int> fileLruList;    
    
    const int maxEntries_;
    const int maxPhysSize_;
    int numEntries_;
    int currentPhysSize_;
    LruReplacePolicy lruPolicy;

};

inline LRUComplexCache::LRUComplexCache(long int capacity, long int size) :
    maxEntries_(capacity),
    maxPhysSize_(size),
    numEntries_(0),
    currentPhysSize_(0)
{
/*    maxEntries_ = capacity;
    maxPhysSize_ = size;
    numEntries_ = 0;
    currentPhysSize_ = 0;*/
    assert(0 < maxEntries_);
    assert(0 < maxPhysSize_);
}

inline LRUComplexCache::~LRUComplexCache()
{
    std::map<int, ComplexEntryType*>::iterator iter;
    for(iter = fileEntryMap.begin(); iter != fileEntryMap.end(); ++iter)
    {
        delete iter->second;
    }
}
inline void LRUComplexCache::insert(const int& handle, 
                                const int& offset, const int& extent)
{
    std::map<int, ComplexEntryType*>::iterator pos;
    pos = fileEntryMap.find(handle);
    /*cerr << "adding mesage "
        << handle << offset << endl;*/
    if(pos->second->handle == handle ||
        pos != fileEntryMap.end()) // if entry already exists
    {
        currentPhysSize_ -= pos->second->blockCache->size();
        pos->second->blockCache->insert(offset, extent);
        currentPhysSize_ += pos->second->blockCache->size();
        /*fileLruList.erase(pos->second->blockLruRef);
        fileLruList.push_front(handle);
        pos->second->blockLruRef = fileLruList.begin();*/
        pos->second->blockLruRef = lruPolicy.PolicyUpdate(&fileLruList,
                            pos->second->blockLruRef, handle, handle);
        
    }else
    {
        ComplexEntryType* entry = new ComplexEntryType;
        entry->handle = handle;
        //printf("inserting handle %d\n", handle);
        if(numEntries_ == maxEntries_)
        {
            int toEvict = lruPolicy.GetEvictIndex(&fileLruList);
            //int toEvict = *(fileLruList.rbegin());
            this->removeHandle(toEvict);
        }
        //printf("current phys size is %d \n", currentPhysSize_);
        while(extent < maxPhysSize_ &&  // check for size constraint
            ((currentPhysSize_+extent) > maxPhysSize_))
        {
            int toEvict = lruPolicy.GetEvictIndex(&fileLruList);
            // int toEvict = *(fileLruList.rbegin());
            // printf("size is %d \n", currentPhysSize_);
            pos = fileEntryMap.find(toEvict);
            this->removeOffset(toEvict, 
                pos->second->blockCache->returnEvict());
            printf(" in complex eviction while loop \n");
        }
        
        // create new blcok cache
        entry->blockCache = (LRUSimpleCache*) malloc(sizeof(LRUSimpleCache));
        entry->blockCache = new LRUSimpleCache(maxEntries_);
        
        // insert into lru list
        /*fileLruList.push_front(handle);
        entry->blockLruRef = fileLruList.begin();*/
        entry->blockLruRef = lruPolicy.PolicyInsert(&fileLruList, handle);
        
        // insert into map and update sizes
        currentPhysSize_ += extent;
        //printf("current phys size is %d \n", currentPhysSize_);
        entry->blockCache->insert(offset, extent);
        fileEntryMap.insert(std::make_pair(handle, entry));
        numEntries_++;
        //printf(" the REAL IZ is %d'n",entry->blockCache->size());
    }

        /*if (numEntries == 1)
    {

    }else if(numEntries == 0)
    {


    }else // if there is two or more entries
    {


    }*/
}


inline void LRUComplexCache::removeHandle(const int& handle)
{
    
    std::map<int, ComplexEntryType*>::iterator pos;
    pos = fileEntryMap.find(handle);
    if(pos != fileEntryMap.end()) // if found
    {
        currentPhysSize_ -= pos->second->blockCache->size();
        fileLruList.erase(pos->second->blockLruRef);
        delete pos->second;
        fileEntryMap.erase(pos->first);
        numEntries_--;
        printf("removing handle %d %d \n", numEntries_, fileEntryMap.size());
    }
}


inline void LRUComplexCache::removeOffset(const int& handle, const int& offset)
{
    std::map<int, ComplexEntryType*>::iterator pos;
    //pos = fileEntryMap.find(handle);
    pos = fileEntryMap.find(handle);
    //if(numEntries_ != 0 && pos!=fileEntryMap.begin()) pos--;
    
    //printf("size before remove is %d %d %d\n", currentPhysSize_,
    //        handle, offset);
    if(pos != fileEntryMap.end()) // if entry found
    {
        printf("removing whole file entry with size %d\n", 
            pos->second->blockCache->physSize());
        currentPhysSize_ -= pos->second->blockCache->physSize();
        pos->second->blockCache->remove(offset);
        currentPhysSize_ += pos->second->blockCache->physSize();
        //printf("size after remove is %d\n", currentPhysSize_);
        if(pos->second->blockCache->size() == 0)
            removeHandle(handle);
    }
}


inline LRUComplexCache::ComplexEntryType* 
LRUComplexCache::lookup(const int& handle, 
                        const int& offset, int extent)
{
    ComplexEntryType* entry = 0;
    if(extent < 1 && extent != -1) extent = 1;
    std::map<int, ComplexEntryType*>::iterator pos;
    if(numEntries_ == 0)
        return entry;
    pos = fileEntryMap.find(handle);
    //printf("gotten handle is %d %d\n", pos->second->handle, handle);
    if(pos->second->handle == handle ||
        pos != fileEntryMap.end()) // found file handle
    {
        if(extent == -1) 
        {    
            entry = pos->second;
            // refresh LRU list
            /*fileLruList.erase(entry->blockLruRef);
            fileLruList.push_front(handle);
            entry->blockLruRef = fileLruList.begin();*/
            entry->blockLruRef = lruPolicy.PolicyUpdate(&fileLruList,
                            entry->blockLruRef, handle, handle);
        }else if(pos->second->blockCache->findOnlyKeyValue(offset,extent)!=0)
        {
            entry = pos->second;
            // refresh LRU list
            /*fileLruList.erase(entry->blockLruRef);
            fileLruList.push_front(handle);
            entry->blockLruRef = fileLruList.begin(); */
            entry->blockLruRef = lruPolicy.PolicyUpdate(&fileLruList,
                            entry->blockLruRef, handle, handle);
        }
    }
    return entry;

}

inline int LRUComplexCache::findOnlyHandle(const int& handle)
{
    int toReturn = 0;
    std::map<int, ComplexEntryType*>::iterator pos;
    if(numEntries_ == 0)
        return 0;
    pos = fileEntryMap.find(handle);
    if(pos != fileEntryMap.end()) // found file handle
        toReturn = 1;    
    return toReturn;
}

inline int LRUComplexCache::findOnlyHandleOffset(const int& handle, 
                        const int& offset)
{
    int toReturn = 0;
    if(numEntries_ == 0)
        return toReturn;
    ComplexEntryType *foundEntry = lookup(handle, offset, -1);
    if(&foundEntry != 0)
        toReturn = 1;
    return toReturn;
}

inline int LRUComplexCache::findOnlyHandleOffsetExtent(const int& handle, 
                                        const int&offset,const int& extent)
{
    int toReturn = 0;
    if(numEntries_ == 0)
        return toReturn;
    ComplexEntryType *foundEntry = lookup(handle, offset, extent);
    if(&foundEntry != 0)
        toReturn = 1;
    return toReturn;
}
#endif

inline int LRUComplexCache::size() const
{

    return numEntries_;
}

inline int LRUComplexCache::physSize() 
{
    return currentPhysSize_;
}

inline void LRUComplexCache::mapPrint()
{
    std::map<int, ComplexEntryType*>::iterator pos;
    printf("printing entire map ----\n");
    for(pos = fileEntryMap.begin(); pos != fileEntryMap.end(); pos++)
    {
        printf("key: %d, size:%d\n", pos->first,
        pos->second->blockCache->physSize());
    }
    printf("total size is %d\n", currentPhysSize_);
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
