#ifndef PFS_UTILS_H
#define PFS_UTILS_H
//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include <map>
#include <vector>
#include "basic_types.h"
class IPvXAddress;
class FSDescriptor;

/** Utility functions for parallel file systems */
class PFSUtils
{
public:
    /** Typedef to for data that a sockets use to connect */
    typedef std::pair<IPvXAddress*, std::size_t> ConnectionDescriptor;
    
    /** Singleton accessor */
    static PFSUtils& instance();

    /** Clear all state information */
    static void clearState();
    
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
