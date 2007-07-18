// File: fifo_replace_policy.cc
#include <map>
#include <cassert>
#include <list>
#include <utility>
#include "fifo_replace_policy.h"

using namespace std;

int FIFOReplacePolicy::GetEvictIndex(map<int, EntryType*> keyEntryMap,
                                list<int> fifoList)
{   
    int toReturn = -1;
    if(fifoList.size() > 0)
    {
        toReturn = *(fifoList.begin());


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
