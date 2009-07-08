//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
