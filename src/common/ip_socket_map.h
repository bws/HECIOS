#ifndef IP_SOCKET_MAP_H
#define IP_SOCKET_MAP_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
