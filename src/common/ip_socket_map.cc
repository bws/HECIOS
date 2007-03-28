#include <cassert>
#include "ip_socket_map.h"
using namespace std;

IPSocketMap::~IPSocketMap()
{
    // Delete the allocated sockets
    map<string, TCPSocket *>::iterator iter;
    for (iter = ipSocketMap_.begin(); iter != ipSocketMap_.end(); ++iter)
    {
        delete iter->second;
        iter->second = 0;
    }
}

void IPSocketMap::addSocket(const string& ipAddr, TCPSocket* socket)
{
    assert(0 != socket);

    // Determine if an entry for this IP already exists
    TCPSocket* entry = getSocket(ipAddr);

    // If the entry exists and isn't the same as this socket, delete the
    // memory
    if (0 != entry && socket != entry)
    {
        delete entry;
    }

    // Add entry
    ipSocketMap_[ipAddr] = socket;
}

void IPSocketMap::removeSocket(const string& ipAddr)
{
    ipSocketMap_.erase(ipAddr);
}

TCPSocket* IPSocketMap::getSocket(const string& ipAddr)
{
    TCPSocket* socket = 0;
    map<string, TCPSocket *>::iterator pos = ipSocketMap_.find(ipAddr);
    if (ipSocketMap_.end() != pos)
    {
        socket = pos->second;
    }
    return socket;

}
