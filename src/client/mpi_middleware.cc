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
//#include "mpi_mid_m.h"
//#include "mpi_middleware_bcast_sm.h"
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
    spfsMPIResponse* response = 0;

    // Create the response based on the message kind
    switch(request->kind())
    {
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
    assert(0 != response);
    send(response, appOutGate_);
}

void MpiMiddleware::initialize()
{
    appInGate_        = findGate("appIn");
    appOutGate_       = findGate("appOut");
    netServerInGate_  = findGate("netServerIn");
    netServerOutGate_ = findGate("netServerOut");
    netClientInGate_  = findGate("netClientIn");
    netClientOutGate_ = findGate("netClientOut");
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
            cerr << __FILE__ << ":" << __LINE__ << ":ERROR: "
                 << "Unknown application request\n";
            assert(false);
        }
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":ERROR: "
             << "MPI Networking simulation is not currently available.\n";
        assert(false);
    }
}

spfsMPIBcastResponse* MpiMiddleware::createBcastResponse(spfsMPIBcastRequest* request) const
{
    spfsMPIBcastResponse* resp =
        new spfsMPIBcastResponse(0, SPFS_MPI_BCAST_RESPONSE);
    resp->setContextPointer(request);
    return resp;
}

/*
// handle other incoming messages (forwarding and processing)
void MpiMiddleware::handleMessage(cMessage* msg)
{
    if(msg->arrivalGateId() == appInGate_)
    {
        switch(msg->kind())
        {
             case SPFS_MPI_BCAST_REQUEST:
                spfsMPIMidBcastRequest *req = new spfsMPIMidBcastRequest(0, SPFS_MPIMID_BCAST_REQUEST);
                req->setRoot(dynamic_cast<spfsMPIBcastRequest *>(msg)->getRoot());
                req->setParent(dynamic_cast<spfsMPIBcastRequest *>(msg)->getRoot());
                req->setCommunicator(dynamic_cast<spfsMPIBcastRequest *>(msg)->getCommunicator());
                req->encapsulate(msg->decapsulate());
                MPIMidBcastSM * sm = new MPIMidBcastSM(this);
                req->setContextPointer(NULL);
                sm->handleMessage(req);
                if(sm->finished())
                {
                    delete sm;
                }
                
                break;
            default:
                cerr << "mpi middleware handleMessage received unknown msg: " << msg->kind() << endl;
                break;
        }
    }

    // Request/Response from other MPI middlewares
    else if(msg->arrivalGateId() == netServerInGate_)
    {
        // handle request / response
        cMessage * ebdMsg = msg->decapsulate();
        assert(ebdMsg != 0);

        switch(ebdMsg->kind())
        {
            case SPFS_MPIMID_BCAST_REQUEST:
            {
                MPIMidBcastSM * sm = new MPIMidBcastSM(this);
                sm->handleMessage(ebdMsg);
                if(sm->finished())
                {
                    delete sm;
                }
                break;
            }
            
            case SPFS_MPIMID_BCAST_RESPONSE:
            {
                MPIMidBcastSM * sm = static_cast<MPIMidBcastSM*>(ebdMsg->contextPointer());
                sm->handleMessage(ebdMsg);
                if(sm->finished())
                {
                    delete sm;
                }
                break;
            }
        }
    }
    delete msg;
}
*/

void MpiMiddleware::sendNet(cMessage *msg)
{
    send(msg, netClientOutGate_);
}

void MpiMiddleware::sendApp(cMessage *msg)
{
    send(msg, appOutGate_);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
