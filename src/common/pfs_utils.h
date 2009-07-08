#ifndef PFS_UTILS_H
#define PFS_UTILS_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <map>
#include <vector>
#include "basic_types.h"
#include "singleton.h"
class IPvXAddress;
class FSDescriptor;

/** Utility functions for parallel file systems */
class PFSUtils : public Singleton<PFSUtils>
{
public:
    /** Enable singleton construction */
    friend class Singleton<PFSUtils>;

    /** Typedef to for data that a sockets use to connect */
    typedef std::pair<IPvXAddress*, std::size_t> ConnectionDescriptor;

    /** Singleton accessor */
    //static PFSUtils& instance();

    /** Clear all state information */
    //static void clearState();

    /** Register a mapping between an IP address and a range of handles */
    void registerServerIP(IPvXAddress* ip, HandleRange range);

    /** Register a mapping between a process rank and a ConnectionDescriptor */
    void registerRankConnectionDescriptor(int rank,
                                          const ConnectionDescriptor& cd);

    /** @return the Server IP address for handle */
    ConnectionDescriptor getRankConnectionDescriptor(int rank) const;

    /** @return the Server IP address for handle */
    IPvXAddress* getServerIP(const FSHandle& handle) const;

    /** @return the Server IP address for handle */
    IPvXAddress* getRankIP(int rank) const;

    /** @return IPs for every rank */
    std::vector<IPvXAddress*> getAllRankIP() const;

private:
    /** Default constructor */
    PFSUtils();

    /** Disabled copy constructor */
    PFSUtils(const PFSUtils& other);

    /** Disabled assignment operator */
    PFSUtils& operator=(const PFSUtils& other);

    /** Register a mapping between a process rank and an IP */
    void registerRankIP(int rank, IPvXAddress* ip);

    /** Singleton instance */
    static PFSUtils* instance_;

    /**
     * Map of handle ranges to IPs -- it is safe to assume that
     * handle ranges do not overlap
     */
    std::map<HandleRange, IPvXAddress*> handleIPMap_;

    /** Map of rank to ConnectionDescriptors */
    std::map<int, ConnectionDescriptor> connectionsByRank_;

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
