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
#include <cassert>
#include <cstring>
#include <iostream>
#include "InterfaceTableAccess.h"
#include "IPv4InterfaceData.h"
#include "IPvXAddress.h"
#include "io_application.h"
#include "mpi_middleware.h"
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

    /** */
    void createTCPApps(cModule* computeNode);

    /** */
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
        
        // Register the rank and IP for each ComputeNode
        long numComputeNodes = cluster->par("numCPUNodes");        
        for (int i = 0; i < numComputeNodes; i++)
        {
            cModule* cpun = cluster->submodule("cpun", i);

            // Alter the TcpApps to be of the correct MPI types
            createTCPApps(cpun);
            
            // Retrieve the IO Application
            cModule* job = cpun->submodule("job");
            assert(0 != job);
            cModule* mpiProcess = job->submodule("mpi");
            assert(0 != mpiProcess);
            IOApplication* ioApp =
                dynamic_cast<IOApplication*>(mpiProcess->submodule("app"));
            assert(0 != ioApp);
            ioApp->initRank();

            MpiMiddleware* mpiMid =
                dynamic_cast<MpiMiddleware*>(mpiProcess->submodule("mpiMiddleware"));
            assert(0 != mpiMid);
            mpiMid->setRank(ioApp->getRank());

            // Register the IP for the this process rank
            IPvXAddress* addr = getComputeNodeIP(cpun);
            PFSUtils::instance().registerRankIP(ioApp->getRank(), addr);

        }
    }
}

void MPIConfigurator::createTCPApps(cModule* computeNode)
{
    assert(0 != computeNode);

    // Retrieve the host
    cModule* hca = computeNode->submodule("hca");
    assert(0 != hca);

    // Retrieve the TCP object
    cModule* tcp = hca->submodule("tcp");
    assert(0 != tcp);

    // Disconnect and delete the existing TcpApp[1] module
    cModule* tcpApp1 = hca->submodule("tcpApp", 1);
    assert(0 != tcpApp1);
    tcpApp1->deleteModule();

    // Create the new MPI TCPApp
    cModuleType* mpiClientType = findModuleType("MPITcpClient");
    assert(0 != mpiClientType);
    cModule* mpiClient = mpiClientType->create("mpiTCPClient", hca);
    mpiClient->buildInside();
    
    // Set the MPITcpClient parameters and connect gates

    mpiClient->par("connectPort") = 6001;
    hca->gate("bmiIn", 1)->connectTo(mpiClient->gate("appIn"));
    mpiClient->gate("appOut")->connectTo(hca->gate("bmiOut", 1));
    tcp->gate("to_appl", 1)->connectTo(mpiClient->gate("tcpIn"));
    mpiClient->gate("tcpOut")->connectTo(tcp->gate("from_appl", 1));
    mpiClient->scheduleStart(simTime());
    mpiClient->callInitialize();
    
    // Convert the third TcpApp into a MPITcpServer
    cModule* tcpApp2 = hca->submodule("tcpApp", 2);
    assert(0 != tcpApp2);
    tcpApp2->deleteModule();

    // Create the new TcpApp MPITcpServer
    cModuleType* mpiServerType = findModuleType("MPITcpServer");
    assert(0 != mpiServerType);
    cModule* mpiServer = mpiServerType->create("mpiTCPServer", hca);
    mpiServer->buildInside();

    // Set the MPITcpServer parameters and connect gates

    mpiServer->par("listenPort") = 6001;
    hca->gate("bmiIn", 2)->connectTo(mpiServer->gate("appIn"));
    mpiServer->gate("appOut")->connectTo(hca->gate("bmiOut", 2));
    tcp->gate("to_appl", 2)->connectTo(mpiServer->gate("tcpIn"));
    mpiServer->gate("tcpOut")->connectTo(tcp->gate("from_appl", 2));
    mpiServer->scheduleStart(simTime());
    mpiServer->callInitialize();
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
