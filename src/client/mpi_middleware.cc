//
// This file is part of Hecios
//
// Copyright (C) 2007 Yang Wu
// Copyright (C) 2008 Yang Wu, Brad Settlemyer
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
#include "mpi_middleware.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include "mpi_communication_helper.h"
#include "mpi_proto_m.h"
using namespace std;

// OMNet Registriation Method
Define_Module(MpiMiddleware);

MpiMiddleware::MpiMiddleware()
    : cSimpleModule(),
      rank_(-1)
{
}

void MpiMiddleware::setRank(int r)
{
    assert(-1 == rank_);
    rank_ = r;
}

int MpiMiddleware::rank() const
{
    assert(rank_ >= 0);
    return rank_;
}

void MpiMiddleware::completeCommunicationCB(spfsMPIRequest* request)
{
    // Do some Omnet Magic to allow the simulation to resume right here
    Enter_Method("Complete MPI Communication");
    take(request);

    spfsMPIResponse* response = 0;

    // Create the response based on the message kind
    switch(request->kind())
    {
        case SPFS_MPI_BARRIER_REQUEST:
        {
            response = createBarrierResponse(
                dynamic_cast<spfsMPIBarrierRequest*>(request));
            break;
        }
        case SPFS_MPI_BCAST_REQUEST:
        {
            response = createBcastResponse(
                dynamic_cast<spfsMPIBcastRequest*>(request));
            break;
        }
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":ERROR "
                 << "Invalid MPI message kind: " << request->kind() << endl;
        }
    }
    send(response, appOutGate_);
    assert(0 != response);
}

void MpiMiddleware::initialize()
{
    // Initialize gates
    appInGate_  = findGate("appIn");
    appOutGate_ = findGate("appOut");
    cacheInGate_  = findGate("cacheIn");
    cacheOutGate_ = findGate("cacheOut");
    netInGate_  = findGate("netIn");
    netOutGate_ = findGate("netOut");
}

void MpiMiddleware::finish()
{
}

void MpiMiddleware::handleMessage(cMessage* msg)
{
    if (msg->arrivalGateId() == appInGate_)
    {
        if (spfsMPICollectiveRequest* coll =
            dynamic_cast<spfsMPICollectiveRequest*>(msg))
        {
            MPICommunicationHelper::instance().performCollective(this, coll);
        }
        else if (spfsMPIRequest* req = dynamic_cast<spfsMPIRequest*>(msg))
        {
            MPICommunicationHelper::instance().performCommunication(this,
                                                                    req,
                                                                    0);
        }
        else
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "Message type not supported." << endl;
            assert(false);
        }
    }
    else if (msg->arrivalGateId() == cacheInGate_)
    {
        send(msg, netOutGate_);
    }
    else if (msg->arrivalGateId() == netInGate_)
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "MPI Message arrived over network: " << msg->info() << endl;
        assert(false);
        send(msg, cacheOutGate_);
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "Message arrived unexpectedly." << endl;
        assert(false);
    }
}

spfsMPIBarrierResponse* MpiMiddleware::createBarrierResponse(
    spfsMPIBarrierRequest* request) const
{
    spfsMPIBarrierResponse* resp =
        new spfsMPIBarrierResponse(0, SPFS_MPI_BARRIER_RESPONSE);
    resp->setContextPointer(request);
    return resp;
}

spfsMPIBcastResponse* MpiMiddleware::createBcastResponse(
    spfsMPIBcastRequest* request) const
{
    spfsMPIBcastResponse* resp =
        new spfsMPIBcastResponse(0, SPFS_MPI_BCAST_RESPONSE);
    resp->setContextPointer(request);
    return resp;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
