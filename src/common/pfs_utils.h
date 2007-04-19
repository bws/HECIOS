#ifndef PFS_UTILS_H
#define PFS_UTILS_H

#include <map>
#include "IPvXAddress.h"
#include "pfs_types.h"

inline bool operator<(HandleRange lhs, HandleRange rhs)
{
    // HandleRange's cannot overlap
    return lhs.first < rhs.first && lhs.last < rhs.last;
}

inline bool operator>(HandleRange lhs, HandleRange rhs)
{
    // HandleRange's cannot overlap
    return lhs.first > rhs.first && lhs.last > rhs.last;
}

/** Utility functions for parallel file systems */
class PFSUtils
{
public:

    /** Singleton accessor */
    static PFSUtils& instance();

    /** Register a mapping between an IP address and a rang of handles */
    void registerServerIP(const IPvXAddress& ip, HandleRange range);
    
    /** @return the Server IP address for handle */
    IPvXAddress getServerIP(const FSHandle& handle) const;
    
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
    std::map<HandleRange, IPvXAddress> handleIPMap;
    
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
