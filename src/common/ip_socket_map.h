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
#include <omnetpp.h>
#include <string.h>
#include <map>
#include "TCPSocket.h"

/** Mapping for IP Addresses to TCPSockets */
class IPSocketMap
{
public:

    /** Default constructor */
    IPSocketMap() {};

    /** Destructor */
    ~IPSocketMap();
    
    /** Add a IP to TCPSocket map entry */
    void addSocket(const std::string& ip, TCPSocket* socket);

    /** Remove the mapping entry for key ip */
    void removeSocket(const std::string& ip);

    /** @return the TCPSocket for key ip, or NULL if ip is not in the map */
    TCPSocket* getSocket(const std::string& ip);

private:

    /** Disable copy construct */
    IPSocketMap(const IPSocketMap& other);
    
    std::map<std::string, TCPSocket *> ipSocketMap_;
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
