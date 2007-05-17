#ifndef LRU_REPLACE_POLICY
#define LRU_REPLACE POLICY
//File lru_replace_policy.h
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
        virtual int GetEvictIndex(map<int, EntryType*> keyEntryMap,
                                        list<int> lruList);
        ~LruReplacePolicy(){}

};


#endif
