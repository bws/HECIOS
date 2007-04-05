#ifndef CLIENT_FS_UTILS_H
#define CLIENT_FS_UTILS_H

#include <ext/hash_map>
#include <list>
#include <string>
#include <vector>
#include <omnetpp.h>
#include "lru_timeout_cache.h"
#include "pfs_types.h"

/**
 * Singleton object for accessing client-side file system utility
 * functions
 */
class ClientFSState
{
public:

    std::vector<HandleRange> servers;
    
    /** Attribute Cache Type */
    typedef LRUTimeoutCache<FSHandle, FSMetaData> AttributeCache;
    
    /** Attribute Cache Entry Type */
    typedef AttributeCache::EntryType AttributeEntry;
    
    /** Directory Cache Type */
    typedef LRUTimeoutCache<std::string, FSHandle> DirectoryCache;
    
    /** Attribute Cache Entry Type */
    typedef DirectoryCache::EntryType DirectoryEntry;
    
    /** The maximum number of entries in the attribute cache */
    static const int MAX_ATTR_ENTRIES = 100;

    /** The maximum time an entry may reside in the attribute cache */    
    static const double MAX_ATTR_TIME = 100.0;

    /** The maximum number of entries in the directory cache*/
    static const int MAX_DIR_ENTRIES = 100;

    /** The maximum number of entries in the directory cache */
    static const double MAX_DIR_TIME = 100.0;

    /** Default constructor */
    ClientFSState();

    /** Add an entry to the attribute cache */
    void insertAttr(FSHandle metaHandle, FSMetaData metaData);

    /** Remove an entry from the attribute cache */
    void removeAttr(FSHandle metaHandle);

    /** @return the meta data for the handle */
    FSMetaData lookupAttr(FSHandle metaHandle);

    /** Add an entry to the directory cache */
    void insertDir(std::string path, FSHandle metaHandle);

    /** Remove an entry from the directory cache */
    void removeDir(std::string path);

    /** @return the meta data handle for the directory */
    FSHandle lookupDir(std::string path);

    /** I have no idea what this function is for (BWS) */
    bool serverNotUsed(int serverNum, int dist, int count, MPIDataType dtype);

    /** called during create to select servers for new file */
    /** randomly selects a server from 0 to S-1 where S is totalNumServers */
    int selectServer();

    /** hashes path to a number from 0 to S-1 where S is totalNumServers */
    int fsHashPath(std::string path);

    /** access function for HandleRange_ vector */
    HandleRange fsServers(int num);

    /** access function for totalNumServers */
    int fsTotalNumServers();

    /** access function for defaultNumServers */
    int fsDefaultNumServers();

    /** access function for root */
    int fsRoot();
    
private:
    /** Copy constructor disabled */
    ClientFSState(ClientFSState& orig);

    /** Attribute cache */
    AttributeCache attrCache_;
    
    /** Directory/Name cache */
    DirectoryCache dirCache_;

    /** Server handle ranges */
    std::vector<HandleRange> handleRanges_;

    /** number of servers in file system */
    int totalNumServers;

    /** default number of servers for 1 file */
    int defaultNumServers;

    /** server with root directory */
    int root;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
