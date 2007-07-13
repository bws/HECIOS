#ifndef PFS_UTILS_H
#define PFS_UTILS_H

#include <map>
#include <vector>
#include "pfs_types.h"
class IPvXAddress;
class Filename;

/** Utility functions for parallel file systems */
class PFSUtils
{
public:

    /** Singleton accessor */
    static PFSUtils& instance();

    /** Clear all state information */
    static void clearState();
    
    /** Register a mapping between an IP address and a rang of handles */
    void registerServerIP(IPvXAddress* ip, HandleRange range);
    
    /** @return the Server IP address for handle */
    IPvXAddress* getServerIP(const FSHandle& handle);

    /** Not sure what this does yet */
    void parsePath(FSOpenFile* descriptor) const;

    /** @return true if the filename exists within the PFS */
    bool fileExists(const Filename& fileName) const;

    /** @return a list of Metadata Server numbers */
    std::vector<int> getMetaServers() const;

    /** @return the number of data servers */
    size_t getNumDataServers() const {return nextServerNumber_;};
    
    /** @return Metadata for a PFS file */
    FSMetaData* getMetaData(const Filename& fileName) const;

    /** @return Descriptor for a PFS file */
    FSOpenFile* getDescriptor(const Filename& fileName) const;

    /** @return the first handle for the server */
    FSHandle getFirstHandle(size_t serverNumber);
    
    /** @return the next handle for server */
    FSHandle getNextHandle(size_t serverNumber);
    
    /** @return the server id for the registering server */
    int registerFSServer(const HandleRange& range, bool isMetaServer);

    /** Create the named directory in the file system */
    void createDirectory(const Filename& dirName, int metaServer);
    
    /** Create the named file in the file system */
    void createFile(const Filename& fileName, int metaServer,
                    int numDataServers);

    /** @return the list of blocks for a file handle */
    std::vector<int> getDiskBlocks(const FSHandle& handle) const;
    
private:

    /** Default constructor */
    PFSUtils();
    
    /** Disabled copy constructor */
    PFSUtils(const PFSUtils& other);

    /** Singleton instance */
    static PFSUtils* instance_;

    /**
     * Map of handle ranges to IPs -- it is safe to assume that
     * handle ranges do not overlap
     */
    std::map<HandleRange, IPvXAddress*> handleIPMap_;

    std::map<std::string, FSHandle> nameToHandleMap_;

    std::map<FSHandle, FSMetaData*> handleToMetaMap_;

    size_t nextServerNumber_;

    std::vector<HandleRange> handlesByServer_;
    
    std::vector<int> metaServers_;

    std::vector<FSHandle> nextHandleByServer_;
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
