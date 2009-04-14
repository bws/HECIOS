#ifndef IP_SOCKET_MAP_H
#define IP_SOCKET_MAP_H
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
#include <cstddef>
#include <string>
#include <map>
#include "TCPSocket.h"

/** Mapping for IP Addresses to TCPSockets */
class IPSocketMap
{
public:
    typedef std::pair<std::string, std::size_t> Connection;

    /**
     * @return -1 if lhs is less than rhs, 0 if they are equal, and 1
     *   if lhs is greater than rhs
     */
    static int compare(const Connection& lhs, const Connection& rhs);

    /** Default constructor */
    IPSocketMap() {};

    /** Destructor */
    ~IPSocketMap();

    /** @return the TCPSocket for key ip, or NULL if ip is not in the map */
    TCPSocket* getSocket(const std::string& ip, std::size_t port) const;

    /** Remove and deallocate all stored sockets */
    void clear();

    /** Add a IP to TCPSocket map entry */
    void addSocket(const std::string& ip, std::size_t port, TCPSocket* socket);

    /** Remove the mapping entry for key ip */
    void removeSocket(const std::string& ip, std::size_t port);

private:

    /** Disable copy construct */
    IPSocketMap(const IPSocketMap& other);

    std::map<Connection, TCPSocket*> ipSocketMap_;
};

inline bool operator==(const IPSocketMap::Connection& lhs,
                       const IPSocketMap::Connection& rhs)
{
    return (0 == IPSocketMap::compare(lhs, rhs));
}

inline bool operator<(const IPSocketMap::Connection& lhs,
                      const IPSocketMap::Connection& rhs)
{
    return (-1 == IPSocketMap::compare(lhs, rhs));
}

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
