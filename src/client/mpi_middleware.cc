//
// This file is part of Hecios
//
// Copyright (C) 2007 Yang Wu
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
#include "cache_proto_m.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "storage_layout_manager.h"
#include "mpi_mid_m.h"
#include "mpi_middleware_bcast_sm.h"
#include <math.h>
#include <iostream>
#include <cassert>

using namespace std;

// OMNet Registriation Method
Define_Module(MpiMiddleware);

// TODO: better way to assign rank
static int rankSeed = 0;


void MpiMiddleware::initialize()
{
    appInGate_        = findGate("appIn");
    appOutGate_       = findGate("appOut");
    netServerInGate_  = findGate("netServerIn");
    netServerOutGate_ = findGate("netServerOut");
    netClientInGate_  = findGate("netClientIn");
    netClientOutGate_ = findGate("netClientOut");
    rank_ = rankSeed++;
    cerr << "middleware: " << rank_ << " initialize"<< endl;
}

void MpiMiddleware::finish()
{
    rankSeed = 0;
}

// handle other incoming messages (forwarding and processing)
void MpiMiddleware::handleMessage(cMessage* msg)
{
    // Message from application
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
                req->setContextPointer(sm);
                sm->handleMessage(req);
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
                ebdMsg->setContextPointer(sm);
                sm->handleMessage(ebdMsg);
                break;
            }
            
            case SPFS_MPIMID_BCAST_RESPONSE:
            {
                MPIMidBcastSM * sm = static_cast<MPIMidBcastSM*>(ebdMsg->contextPointer());
                sm->handleMessage(ebdMsg);                
                break;
            }
        }
    }
    delete msg;
}

int MpiMiddleware::getRank()
{
    return rank_;
}

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
