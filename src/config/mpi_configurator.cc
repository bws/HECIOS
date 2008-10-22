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
    /** @return the IP address registered to the compute node */
    IPvXAddress* getComputeNodeIP(cModule* computeNode);

    /** Construct all the correct TCPApp's for each process */
    void connectTCPApp(cModule* computeNode, size_t processIdx);

    /** Construct all the correct TCPApp's for each process */
    void createTCPApps(cModule* computeNode, size_t numProcs);

    /** Construct an MPI TCP Client and Server for the process index */
    void createMPIApps(cModule* computeNode,
                       size_t processIdx,
                       size_t listenPort);

    /** The lowest possible port for the MPI Server to listen on */
    size_t minListenPort_;

    /** The highest possible port for the MPI Server to listen on */
    size_t maxListenPort_;

    /** The next port to assign for an MPI server to listen on */
    size_t nextPortToAssign_;

    /** The next process rank to assign to an MPI process */
    size_t nextProcessRank_;
};

// OMNet Registriation Method
Define_Module(MPIConfigurator);

/**
 * Initialization - Set the handle ranges and register the assigned IP address
 */
void MPIConfigurator::initialize(int stage)
{
    // Stage based initialization
    if (0 == stage)
    {
        // Local initialization Stage 0
        minListenPort_ = par("listenPortMin");
        maxListenPort_ = par("listenPortMax");
        nextPortToAssign_ = minListenPort_;
        cerr << "DIAGNOSTIC: Max number of MPI processes is: "
             << maxListenPort_ - minListenPort_ << endl;

        // Initializae the next rank to 0
        nextProcessRank_ = 0;
    }
    else if (3 == stage)
    {
        // Stage 4 initialiazation should ensure that IP addresses have been
        // assigned

        // Retrieve the interface table for this module
        cModule* cluster = parentModule();
        assert(0 != cluster);

        // Register the rank and IP for each ComputeNode
        long numComputeNodes = cluster->par("numCPUNodes");
        for (int i = 0; i < numComputeNodes; i++)
        {
            cModule* cpun = cluster->submodule("cpun", i);
            assert(0 != cpun);

            // Retrieve the IP address for this compute node
            IPvXAddress* addr = getComputeNodeIP(cpun);

            // Retrieve the number of job processes on the cpu node
            size_t numProcesses = cpun->par("numProcs");

            for (size_t j = 0; j < numProcesses; j++)
            {
                // Retrieve the IO Application
                cModule* process = cpun->submodule("process", j);
                assert(0 != process);
                cModule* mpiProcess = process->submodule("mpi");
                assert(0 != mpiProcess);
                IOApplication* ioApp =
                    dynamic_cast<IOApplication*>(mpiProcess->submodule("app"));
                assert(0 != ioApp);

                // Set the rank for the IO Application
                size_t rank = nextProcessRank_++;
                ioApp->setRank(rank);

                // Connect the application process to a TCP application
                size_t listenPort = nextPortToAssign_++;
                connectTCPApp(cpun, j);

                // Register the IP/port for the this process rank
                PFSUtils::instance().registerRankConnectionDescriptor(
                    rank, make_pair(addr, listenPort));

                // Set the rank for the MPI Middleware
                MpiMiddleware* mpiMid = dynamic_cast<MpiMiddleware*>(
                    mpiProcess->submodule("mpiMiddleware"));
                assert(0 != mpiMid);
                mpiMid->setRank(rank);
            }
        }
    }
}

/**
 * Disable message handling
 */
void MPIConfigurator::handleMessage(cMessage* msg)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "ERROR: MPIConfigurator cannot receive messages!!!" << endl;
    assert(false);
}

void MPIConfigurator::connectTCPApp(cModule* computeNode, size_t procIdx)
{
    assert(0 != computeNode);

    // Retrieve the host
    cModule* hca = computeNode->submodule("hca");
    assert(0 != hca);
}

void MPIConfigurator::createTCPApps(cModule* computeNode, size_t numProcs)
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

void MPIConfigurator::createMPIApps(cModule* computeNode,
                                    size_t processIdx,
                                    size_t listenPort)
{
    assert(0 != computeNode);
    assert(listenPort >= minListenPort_);
    assert(listenPort <= maxListenPort_);

    // Retrieve the host
    cModule* hca = computeNode->submodule("hca");
    assert(0 != hca);

    // Retrieve the TCP object
    cModule* tcp = hca->submodule("tcp");
    assert(0 != tcp);

    // The TCPApp array for compute nodes contains 3 entries for each
    // process index
    //
    // By default all of the TCPApps are BMITCPClient's
    // tcpApp[processIdx*3] = BMITCPClient
    // tcpApp[processIdx*3 + 1] = MPITCPServer
    // tcpApp[processIdx*3 + 2] = MPITCPClient

    // Delete the existing tcpApp[rank*3 + 1]
    size_t mpiServerIdx = processIdx*3 + 1;
    cModule* tcpApp1 = hca->submodule("tcpApp", mpiServerIdx);
    assert(0 != tcpApp1);
    tcpApp1->deleteModule();

    // Create the new TcpApp MPITcpServer
    cModuleType* mpiServerType = findModuleType("MPITcpServer");
    assert(0 != mpiServerType);
    cModule* mpiServer = mpiServerType->create("mpiTCPServer", hca);
    mpiServer->buildInside();

    // Set the MPITcpServer parameters and connect gates
    mpiServer->par("listenPort") = listenPort;
    hca->gate("bmiIn", mpiServerIdx)->connectTo(mpiServer->gate("appIn"));
    mpiServer->gate("appOut")->connectTo(hca->gate("bmiOut", mpiServerIdx));
    tcp->gate("to_appl", mpiServerIdx)->connectTo(mpiServer->gate("tcpIn"));
    mpiServer->gate("tcpOut")->connectTo(tcp->gate("from_appl", mpiServerIdx));
    mpiServer->scheduleStart(simTime());
    mpiServer->callInitialize();


    // Disconnect and delete the existing TcpApp[processIdx*3 + 2] module
    size_t mpiClientIdx = processIdx*3 + 2;
    cModule* tcpApp2 = hca->submodule("tcpApp", mpiClientIdx);
    assert(0 != tcpApp2);
    tcpApp2->deleteModule();

    // Create the new MPI Client TCPApp
    cModuleType* mpiClientType = findModuleType("MPITcpClient");
    assert(0 != mpiClientType);
    cModule* mpiClient = mpiClientType->create("mpiTCPClient", hca);
    mpiClient->buildInside();

    // Set the MPITcpClient parameters and connect gates
    hca->gate("bmiIn", mpiClientIdx)->connectTo(mpiClient->gate("appIn"));
    mpiClient->gate("appOut")->connectTo(hca->gate("bmiOut", mpiClientIdx));
    tcp->gate("to_appl", mpiClientIdx)->connectTo(mpiClient->gate("tcpIn"));
    mpiClient->gate("tcpOut")->connectTo(tcp->gate("from_appl", mpiClientIdx));
    mpiClient->scheduleStart(simTime());
    mpiClient->callInitialize();
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
