// File: CacheEntry.cc
// Purpose: to implement a cache entry object to be created
//          by the cache for use in the cache

#include "cache_entry.h"

enum cacheState
{
    EXCLUSIVE,
    VALID,
    INVALID,
    UNKNOWN;
}

// cache entry constructor
CacheEntry::CacheEntry(int inAddress, int inExtent, unsigned char inState)
{
	address = inAddress;
	extent = inExtent;
	entryState = inState;

}

CacheEntry::changeEntryState(unsigned char newState)
{
	entryState = newState;

}

CacheEntry::changeEntryExtent(int newExtent)
{
	extent = newExtent;
}

CacheEntry::changeEntryAddress(int newAddress)
{
	address = newAddress;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
