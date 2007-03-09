#ifndef IP_SOCKET_MAP_H
#define IP_SOCKET_MAP_H
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
