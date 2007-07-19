#include <cassert>
#include <cstring>
#include <iostream>
#include "InterfaceTableAccess.h"
#include "IPv4InterfaceData.h"
#include "IPvXAddress.h"
#include "io_application.h"
#include "pfs_types.h"
#include "pfs_utils.h"
#include <omnetpp.h>
using namespace std;

/**
 * Configurator model for MPI processes
 */
class MPIConfigurator : public cSimpleModule
{
public:
    /** Constructor */
    MPIConfigurator() : cSimpleModule() {};
    
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

    IPvXAddress* getComputeNodeIP(cModule* computeNode);
};

// OMNet Registriation Method
Define_Module(MPIConfigurator);

/**
 * Initialization - Set the handle ranges and register the assigned IP address
 */
void MPIConfigurator::initialize(int stage)
{
    // Stage 4 initialiazation should ensure that IP addresses have been
    // assigned
    if (3 == stage)
    {
        // Retrieve the interface table for this module
        cModule* cluster = parentModule();
        assert(0 != cluster);

        // Register the handles and IP for each ComputeNode
        long numComputeNodes = cluster->par("numCPUNodes");        
        for (int i = 0; i < numComputeNodes; i++)
        {
            cModule* cpun = cluster->submodule("cpun", i);

            // Retrieve the IO Application
            cModule* job = cpun->submodule("jobProcess");
            assert(0 != job);
            cModule* mpiProcess = job->submodule("mpi");
            assert(0 != mpiProcess);
            IOApplication* ioApp = dynamic_cast<IOApplication*>(mpiProcess->submodule("app"));
            assert(0 != ioApp);

            // Register the IP for the this process rank
            IPvXAddress* addr = getComputeNodeIP(cpun);
            PFSUtils::instance().registerRankIP(ioApp->getRank(), addr);

        }
    }
}

IPvXAddress* MPIConfigurator::getComputeNodeIP(cModule* computeNode)
{
    assert(0 != computeNode);
    IPvXAddress* serverIP = 0;
    
    // Retrieve the INET host
    cModule* hca = computeNode->submodule("hca");
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
void MPIConfigurator::handleMessage(cMessage* msg)
{
    cerr << "MPIConfigurator cannot receive messages." << endl;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
