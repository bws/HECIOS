#ifndef PFS_UTILS_H
#define PFS_UTILS_H

#include <map>
#include <vector>
#include "basic_types.h"
class IPvXAddress;
class FSDescriptor;

/** Utility functions for parallel file systems */
class PFSUtils
{
public:

    /** Singleton accessor */
    static PFSUtils& instance();

    /** Clear all state information */
    static void clearState();
    
    /** Register a mapping between an IP address and a range of handles */
    void registerServerIP(IPvXAddress* ip, HandleRange range);
    
    /** @return the Server IP address for handle */
    IPvXAddress* getServerIP(const FSHandle& handle) const;

    /** Register a mapping between a process rank and an IP */
    void registerRankIP(int rank, IPvXAddress* ip);
    
    /** @return the Server IP address for handle */
    IPvXAddress* getRankIP(int rank) const;

    /** @return all server IPs */
    std::vector<IPvXAddress*> getAllRankIP() const;
    
    /** Not sure what this does yet */
    void parsePath(FSDescriptor* descriptor) const;

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

    std::map<int, IPvXAddress*> ipsByRank_;
    
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
