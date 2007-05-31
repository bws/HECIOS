#ifndef LRU_REPLACE_POLICY_H
#define LRU_REPLACE_POLICY_H
// File lru_replace_policy.h
// 
#include <map>
#include <cassert>
#include <list>
#include <utility>
#include "replace_policy.h"

using namespace std;

class LruReplacePolicy:public ReplacePolicy
{
    public:
		typedef LRUSimpleCacheEntry EntryType;	
        // virtual int GetEvictIndex(map<int, EntryType*> keyEntryMap,
        //                                list<int> lruList){return -1;}
		// int AFunction(int i){ return 1;}
        ~LruReplacePolicy(){}

};


#endif
