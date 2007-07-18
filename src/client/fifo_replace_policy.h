#ifndef FIFO_REPLACE_POLICY_H
#define FIFO_REPLACE_POLICY_H
// File FIFO_replace_policy.h
// 
#include <map>
#include <cassert>
#include <list>
#include <utility>
#include "replace_policy.h"

using namespace std;

class FIFOReplacePolicy:public ReplacePolicy
{
    public:
		typedef FIFOSimpleCacheEntry EntryType;
		int GetEvictIndex(map<int, EntryType*> keyEntryMap,
                                        list<int> *lruList)
        {
            int toReturn = -1;
            if((*lruList).size() > 0)
            {
                toReturn = *((*lruList).rbegin());
            }
            printf("returning value %d for eviction\n", toReturn);
            return toReturn;

        }

        list<int>::iterator (list<int> *lruList, list<int>::iterator *lruRef,
                                    int key, int toDelete)
        {
            return lruRef;
        }
        list<int>::iterator PolicyInsert(list<int> *lruList, int key)
        {

            (*lruList).push_front(key);
		 	return (*lruList).begin();
		}
			
        // virtual int GetEvictIndex(map<int, EntryType*> keyEntryMap,
        //                                list<int> lruList){return -1;}
		// int AFunction(int i){ return 1;}
        ~FIFOReplacePolicy(){}

};


#endif
