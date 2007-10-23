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
            appBcastHandler(dynamic_cast<spfsMPIBcastRequest *>(msg));
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
                spfsMPIMidBcastRequest * bcastReq = dynamic_cast<spfsMPIMidBcastRequest*>(ebdMsg);
                assert(bcastReq != 0);
                
                MPIMidBcastSM * bcastSm = new MPIMidBcastSM(this); // bcastsm will be self destructed after doing his job
                bcastSm->handleMessage(bcastReq);

                break;
            }
            
        case SPFS_MPIMID_BCAST_RESPONSE:
            {
                spfsMPIMidBcastResponse * bcastRsp = dynamic_cast<spfsMPIMidBcastResponse*>(ebdMsg);
                assert(bcastRsp != 0);
                
                MPIMidBcastSM * bcastSm2 = static_cast<MPIMidBcastSM*>((void*)bcastRsp->getUniId());
                assert(bcastSm2 != 0);
                
                bcastSm2->handleMessage(bcastRsp);
                
                break;
            }
        }
    }
    delete msg;
}

// handling bcast request from application
void MpiMiddleware::appBcastHandler(spfsMPIBcastRequest* mpiReq)
{
    assert(mpiReq != 0);

    vector<IPvXAddress*> ips = PFSUtils::instance().getAllRankIP();

    // Sequentially assigning ranks to comm member array if it is a global bcasting
    if(mpiReq->getIsGlobal())
    {
        unsigned int node_num = ips.size();
        mpiReq->setCommMembersArraySize(node_num);
        
        for(unsigned int i = 0; i < node_num; i++)
        {
            mpiReq->setCommMembers(i, i);
        }
    }

    // Preparing the mpi mid bcast request msg
    cMessage *req = mpiReq->decapsulate();
    assert(req);
  
    spfsMPIMidBcastRequest *mpiMidReq = new spfsMPIMidBcastRequest("Mpimid Bcast", SPFS_MPIMID_BCAST_REQUEST);
    mpiMidReq->encapsulate(req);
    mpiMidReq->setStep(-1);
    mpiMidReq->setRoot(-1);

    // Get comm members from bcast msg sent by app, and write into mpi mid bcast msg
    int rankSize = mpiReq->getCommMembersArraySize();
    mpiMidReq->setRankSetArraySize(rankSize);
    for(int i = 0; i < rankSize; i++)
    {
        int rank = mpiReq->getCommMembers(i);
        if(rank_ == mpiReq->getRoot())
            mpiMidReq->setRoot(i);
        mpiMidReq->setRankSet(i, rank);

        // change the comm member[0] to self rank, the algorithm starts bcasting from rank 0
        if(rank_ == rank)
        {
            mpiMidReq->setRankSet(i, mpiMidReq->getRankSet(0));
            mpiMidReq->setRankSet(0, rank);
        }
    }

    MPIMidBcastSM * bcastSm = new MPIMidBcastSM(this); // bcastSm will be self destructed after doing his job
    bcastSm->handleMessage(mpiMidReq);
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
