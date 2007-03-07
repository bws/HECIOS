// File: CacheEntry.h
// Purpose: to implement a cache entry class to be created
//          by the cache for use in the cache


class CacheEntry{
public:
    CacheEntry(int inAddress, int inExtent, unsigned char inState);
    void changeEntryState(unsigned char newState);
    void changeEntryExtent(int newExtent);
    void changeEntryAddress(int newAddress);

private:
    int address;
    int extent;
    unsigned char entryState;
};

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
