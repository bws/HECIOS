//
// This file is part of Hecios
//
// Copyright (C) 2007 Pooja Verma
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
#include "ip_socket_map.h"
#include <cassert>
using namespace std;

IPSocketMap::~IPSocketMap()
{
    // Sockets are currently shared, this does not need to clear the sockets
    // clear();
}

void IPSocketMap::clear()
{
    // Delete the allocated sockets
    map<Connection, TCPSocket *>::iterator iter;
    for (iter = ipSocketMap_.begin(); iter != ipSocketMap_.end(); ++iter)
    {
        delete iter->second;
        iter->second = 0;
    }
}

void IPSocketMap::addSocket(const string& ipAddr, size_t port, TCPSocket* socket)
{
    assert(0 != socket);

    // Determine if an entry for this IP already exists
    TCPSocket* entry = getSocket(ipAddr, port);

    // If the entry exists and isn't the same as this socket, delete the
    // memory
    if (0 != entry && socket != entry)
    {
        delete entry;
    }

    // Add entry
    Connection conn = make_pair(ipAddr, port);
    ipSocketMap_[conn] = socket;
}

void IPSocketMap::removeSocket(const string& ipAddr, size_t port)
{
    Connection conn = make_pair(ipAddr, port);
    map<Connection, TCPSocket*>::iterator itr = ipSocketMap_.find(conn);
    if (ipSocketMap_.end() != itr)
    {
       ipSocketMap_.erase(conn);
    }
}

TCPSocket* IPSocketMap::getSocket(const string& ipAddr, size_t port) const
{
    TCPSocket* socket = 0;
    Connection conn = make_pair(ipAddr, port);
    map<Connection, TCPSocket*>::const_iterator pos = ipSocketMap_.find(conn);
    if (ipSocketMap_.end() != pos)
    {
        socket = pos->second;
    }
    return socket;

}

int IPSocketMap::compare(const Connection& lhs, const Connection& rhs)
{
    int compareVal = 1;
    if (lhs.second == rhs.second)
    {
        if (lhs.first < rhs.first)
        {
            compareVal = -1;
        }
        else if (lhs.first == rhs.first)
        {
            compareVal = 0;
        }
    }
    else if (lhs.second < rhs.second)
    {
        compareVal = -1;
    }
    return compareVal;
}
