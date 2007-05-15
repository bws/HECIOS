#include <cassert>
#include <cstring>
#include <iostream>
#include "InterfaceTableAccess.h"
#include "IPv4InterfaceData.h"
#include "IPvXAddress.h"
#include "fs_server.h"
#include "mpiio_proto_m.h"
#include "pvfs_proto_m.h"
#include "pfs_types.h"
#include "pfs_utils.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class FSServerConfigurator : public cSimpleModule
{
public:
    /** Constructor */
    FSServerConfigurator() : cSimpleModule() {};
    
protected:

    /** Must have more stages than it takes to assign IPs */
    virtual int numInitStages() const {return 4;};
    
    /** Implementation of initialize */
    virtual void initialize(int stage);

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:

    IPvXAddress* getServerIP(cModule* ioNode);
};

// OMNet Registriation Method
Define_Module(FSServerConfigurator);

/**
 * Initialization - Set the handle ranges and register the assigned IP address
 */
void FSServerConfigurator::initialize(int stage)
{
    // Stage 4 initialiazation should ensure that IP addresses have been
    // assigned
    if (3 == stage)
    {
        // Retrieve the interface table for this module
        cModule* cluster = parentModule();
        assert(0 != cluster);

        // Register the handles and IP for each IONode
        long numIONodes = cluster->par("numIONodes");        
        for (int i = 0; i < numIONodes; i++)
        {
            cModule* ion = cluster->submodule("ion", i);

            // Retrieve the FS server
            cModule* daemon = ion->submodule("daemon");
            assert(0 != daemon);
            FSServer* server =
                dynamic_cast<FSServer*>(daemon->submodule("server"));
            assert(0 != server);

            // Register the server's handle range
            PFSUtils& utils = PFSUtils::instance();
            int serverNum;
            if (0 == i)
            {
                serverNum = utils.registerFSServer(server->getHandleRange(),
                                                   true);
            }
            else
            {
                serverNum = utils.registerFSServer(server->getHandleRange(),
                                                   false);
            }
            
            // Set the server's server number
            server->setNumber(serverNum);

            // Register the IP for the handle range
            IPvXAddress* addr = getServerIP(ion);
            utils.registerServerIP(addr, server->getHandleRange());

        }
    }
}

IPvXAddress* FSServerConfigurator::getServerIP(cModule* ioNode)
{
    assert(0 != ioNode);
    IPvXAddress* serverIP = 0;
    
    // Retrieve the INET host
    cModule* hca = ioNode->submodule("hca");
    assert(0 != hca);
    InterfaceTable* ifTable = dynamic_cast<InterfaceTable*>(
        hca->submodule("interfaceTable"));
    assert(0 != ifTable);
            
    // Find eth0's IP address
    InterfaceEntry* ie = 0;
    for (int j = 0; j < ifTable->numInterfaces(); j++)
    {
        ie = ifTable->interfaceAt(j);
        if (0 == strcmp("eth0", ie->name()))
        {
            assert(0 != ie->ipv4());
            serverIP = new IPvXAddress(ie->ipv4()->inetAddress());
            break;
        }
    }

    return serverIP;
}

/**
 * Disable message handling
 */
void FSServerConfigurator::handleMessage(cMessage* msg)
{
    cerr << "FSServerConfigurator cannot receive messages." << endl;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
