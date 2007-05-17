
// File: lru_replace_policy.cc
#include <map>
#include <cassert>
#include <list>
#include <utility>
//#include "replace_policy.h"
#include "lru_replace_policy.h"

using namespace std;

//template <class KeyType, class ValueType>
/*class LruReplacePolicy : public ReplacePolicy
{
    public:
        virtual int GetEvictIndex(map<int, int> keyEntryMap,
                                        list<int> lruList);
        ~LruReplacePolicy(){};

};*/

/*LruReplacePolicy::~LruReplacePolicy()
{
    // does nothing, needed so compiler doesn't complain

}*/

/*ReplacePolicy::~ReplacePolicy()
{

}*/

//template <class KeyType, class ValueType>
int LruReplacePolicy::GetEvictIndex(map<int, EntryType*> keyEntryMap,
                                list<int> lruList)
{   
    int toReturn = -1;
    if(lruList.size() > 0)
    {
        toReturn = *(lruList.begin());


    }
    printf("returning value %d\n", toReturn);
    return toReturn;

}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
