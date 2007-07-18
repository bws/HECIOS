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
        int GetEvictIndex(list<int> *lruList)
		{
			int toReturn = -1;
    		if((*lruList).size() > 0)
    		{
        		toReturn = *((*lruList).rbegin());
    		}
		    //printf("returning value %d\n", toReturn);
    		return toReturn;

		}
		
		list<int>::iterator PolicyUpdate(list<int> *lruList, 
						list<int>::iterator &lruRef, int key,
						int toDelete)
		{
			//(*lruList).erase(lruRef);
			(*lruList).remove(toDelete);
			return PolicyInsert(lruList, key);
		}
        list<int>::iterator PolicyInsert(list<int> *lruList, int key)
		{

			(*lruList).push_front(key);
			return (*lruList).begin();	

		}

        ~LruReplacePolicy(){}

};


#endif
