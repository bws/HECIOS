// File: CacheEntry.cc
// Purpose: to implement a cache entry object to be created
//          by the cache for use in the cache


class CacheEntry{
public:
	CacheEntry(int inAddress, int inData, unsigned char inState);
	changeEntryState(unsigned char newState);
	changeEntryData(int newData);
	changeEntryAddress(int newAddress);

private:
	int address;
	int data;
	unsigned char entryState;
};

// cache entry constructor
CacheEntry::CacheEntry(int inAddress, int inData, unsigned char inState)
{
	address = inAddress;
	data = inData;
	entryState = inState;

}

CacheEntry::changeEntryState(unsigned char newState)
{
	entryState = newState;

}

CacheEntry::changeEntryData(int newData)
{
	data = newData;
}

CacheEntry::changeEntryAddress(int newAddress)
{
	address = newAddress;
}
