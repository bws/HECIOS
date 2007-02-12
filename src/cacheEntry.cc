// File: CacheEntry.cc
// Purpose: to implement a cache entry object to be created
//          by the cache for use in the cache


class CacheEntry{
public:
	CacheEntry(int inAddress, int inExtent, unsigned char inState);
	changeEntryState(unsigned char newState);
	changeEntryExtent(int newExtent);
	changeEntryAddress(int newAddress);

private:
	int address;
	int extent;
	unsigned char entryState;
};

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
