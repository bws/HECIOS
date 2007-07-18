#ifndef REPLACE_POLICY_H
#define REPLACE_POLICY_H

//Name: replace_policy.h
//Purpose: to provide an abstract class for policy based replacement
#include <map>
#include <cassert>
#include <list>
#include <utility>
#include <omnetpp.h>

// setup namespace
using namespace std;

/**
 * A CacheEntry wrapper that includes a simulation timestamp
 */
//template <class KeyType, class ValueType>
//template <class KeyType, class ValueType>
struct LRUSimpleCacheEntry
{
    int extent;
    int offset;
    int address;
    int state;
    double timeStamp;
    std::list<int>::iterator lruRef;
};

//template <class KeyType, class ValueType>
class ReplacePolicy
{

	//virtual void Insert(int address, int extent);
	//virtual void Remove(int address);
	//virtual void Remove(int address, int extent);
	//virtual void lookup(int address);
    public:
        typedef LRUSimpleCacheEntry EntryType;
        virtual ~ReplacePolicy();
	/*virtual int GetEvictIndex(map <int, EntryType*> keyEntryMap,
                                        list<int> *lruList) = 0;*/
	virtual int GetEvictIndex(list<int> *lruList) = 0;
        virtual list<int>::iterator PolicyUpdate(list<int> *lruList, 
                list<int>::iterator &lruRef, int key, int toDelete)=0;
        virtual list<int>::iterator PolicyInsert(list<int> *lruList, int key)=0;
        /*{
            printf("have to put this here b/c gcc linker sucks\n");
            int toreturn = *((*lruList).rbegin());
            printf("to return is %d\n", toreturn);
            return *((*lruList).rbegin());
            //return 1;
        }*/
};


inline ReplacePolicy::~ReplacePolicy()
{
    //printf("nothing to do here");


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
