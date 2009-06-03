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
#include <set>
#include "InterfaceTableAccess.h"
#include "IPv4InterfaceData.h"
#include "IPvXAddress.h"
#include "io_application.h"
#include "middleware_aggregator.h"
#include "middleware_cache.h"
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
    /** @return the next rank to assign */
    size_t getNextRank() const;

    /** @return the IP address registered to the compute node */
    IPvXAddress* getComputeNodeIP(cModule* computeNode);

    /** @return the listen port for the mpi server */
    size_t getMPIServerListenPort(cModule* computeNode, size_t processIdx);

    /** Construct all the correct TCPApp's for each process */
    void setMPIServerListenPort(cModule* computeNode,
                                size_t processIdx,
                                size_t processListenPort);

    /** The lowest possible port for the MPI Server to listen on */
    size_t minListenPort_;

    /** The highest possible port for the MPI Server to listen on */
    size_t maxListenPort_;

    /** The next port to assign for an MPI server to listen on */
    size_t nextPortToAssign_;

    /** The next process rank to assign to an MPI process */
    mutable size_t nextProcessRank_;

    /** Determine if ranks should be randomized */
    bool randomizeRanks_;

    /** The total number of processes to assign */
    size_t totalProcessCount_;
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

        // Initialize the rank setting variables
        randomizeRanks_ = par("randomizeRanks");
        nextProcessRank_ = 0;
        totalProcessCount_ = 0;

        // Set the listen ports for the servers
        cModule* cluster = parentModule();
        assert(0 != cluster);
        size_t numComputeNodes = cluster->par("numCPUNodes");
        for (size_t i = 0; i < numComputeNodes; i++)
        {
            // Retrieve the number of job processes on the cpu node
            cModule* cpun = cluster->submodule("cpun", i);
            assert(0 != cpun);
            size_t numProcesses = cpun->par("numProcs");
            for (size_t j = 0; j < numProcesses; j++)
            {
                // Set the port for the MPI Server
                size_t listenPort = nextPortToAssign_++;
                setMPIServerListenPort(cpun, j, listenPort);
            }

            // Update bookkeeping info
            totalProcessCount_ += numProcesses;
        }
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
                size_t rank = getNextRank();
                ioApp->setRank(rank);

                // Set the rank for the MPI Middleware
                MpiMiddleware* mpiMid = dynamic_cast<MpiMiddleware*>(
                    mpiProcess->submodule("mpiMiddleware"));
                assert(0 != mpiMid);
                mpiMid->setRank(rank);

                // Set the rank and size for the aggregator middleware
                MiddlewareAggregator* mwAgg = dynamic_cast<MiddlewareAggregator*>(
                    mpiProcess->submodule("aggregator"));
                assert(0 != mwAgg);
                mwAgg->setRank(rank);
                mwAgg->setAggregatorSize(numProcesses);

                // Set the rank for the cache middleware
                MiddlewareCache* mwCache = dynamic_cast<MiddlewareCache*>(
                    mpiProcess->submodule("cache"));
                assert(0 != mwCache);
                mwCache->setRank(rank);

                // Set the port for the MPI Server
                size_t listenPort = getMPIServerListenPort(cpun, j);

                // Register the IP/port for the this process rank
                PFSUtils::instance().registerRankConnectionDescriptor(
                    rank, make_pair(addr, listenPort));
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

size_t MPIConfigurator::getNextRank() const
{
    size_t nextRank;
    if (randomizeRanks_)
    {
        // Select a random rank from a hat
        static vector<size_t> rankHat;
        if (rankHat.empty())
        {
            assert(0 == nextProcessRank_);
            for (size_t i = 0; i < totalProcessCount_; i++)
            {
                rankHat.push_back(i);
            }
        }
        int idx = intuniform(0, rankHat.size() - 1);
        nextRank = rankHat[idx];
        rankHat.erase(rankHat.begin() + idx);
        nextProcessRank_++;
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "DIAGNOSTIC: random rank assigned: " << nextRank << endl;
    }
    else
    {
        nextRank = nextProcessRank_++;
    }
    return nextRank;
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

size_t MPIConfigurator::getMPIServerListenPort(cModule* computeNode,
                                               size_t processIdx)
{
    assert(0 != computeNode);

    // Retrieve the host
    cModule* hca = computeNode->submodule("hca");
    assert(0 != hca);

    // Retrieve the TCP object
    cModule* tcp = hca->submodule("tcp");
    assert(0 != tcp);

    // Retrieve the MPITcpServer
    cModule* mpiServer = hca->submodule("mpiTcpApp", processIdx);
    assert(0 != mpiServer);

    return mpiServer->par("listenPort").longValue();
}

void MPIConfigurator::setMPIServerListenPort(cModule* computeNode,
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

    // Retrieve the MPITcpServer
    cModule* mpiServer = hca->submodule("mpiTcpApp", processIdx);
    assert(0 != mpiServer);

    // Set the MPITcpServer's listen port
    mpiServer->par("listenPort") = listenPort;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
