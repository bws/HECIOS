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

#include <omnetpp.h>
#include "mpi_middleware_bcast_sm.h"
#include "mpi_middleware.h"
#include "mpi_mid_m.h"
#include "mpi_proto_m.h"
#include <iostream>
#include <cassert>

using namespace std;

MPIMidBcastSM::MPIMidBcastSM(MpiMiddleware *mpi_mid)
{
    mpiMiddleware_ = mpi_mid;
    rankNum_ = 0;
    rspNum_ = 0;
    rspUniId_ = -1;
    rspRank_ = -1;
}

void MPIMidBcastSM::handleMessage(cMessage* msg)
{
    enum
    {
        INIT = 0,
        BCAST = FSM_Transient(1),
        PREWAIT = FSM_Steady(2),
        WAIT = FSM_Transient(3),
        RSP = FSM_Steady(4),
    };

    FSM_Switch(currentState_)
    {
    case FSM_Exit(INIT):
        {
            spfsMPIMidBcastRequest* req = dynamic_cast<spfsMPIMidBcastRequest*>(msg);
            assert(0 != req);
            
            req = NULL;
            FSM_Goto(currentState_, BCAST);
            
            break;
        }
    
    case FSM_Enter(BCAST):
        {   
            spfsMPIMidBcastRequest* req = dynamic_cast<spfsMPIMidBcastRequest*> (msg);
            assert(0 != req);
            
            enterBcast(req);

            break;
        }
    
    case FSM_Exit(BCAST):
        {
            if(rankNum_ != 0)
                FSM_Goto(currentState_, PREWAIT);
            else
                FSM_Goto(currentState_, RSP);
            break;
        }
    
    case FSM_Enter(PREWAIT):
        {
            break;
        }
    
    case FSM_Exit(PREWAIT):
        {
            FSM_Goto(currentState_, WAIT);
            break;
        }
    
    case FSM_Enter(WAIT):
        {
            if(spfsMPIMidBcastResponse* rsp = dynamic_cast<spfsMPIMidBcastResponse*> (msg))
                enterWait(rsp);
            break;
        }
    
    case FSM_Exit(WAIT):
        {
            if(rspNum_ == rankNum_)
                FSM_Goto(currentState_, RSP);
            else
                FSM_Goto(currentState_, PREWAIT);
            break;
        }
    
    case FSM_Enter(RSP):
        {
            enterRsp();
            break;
        }
    default:
        {
            cerr << "Dear god what went wrong" << endl;
            break;
        }
    }
}

void MPIMidBcastSM::enterBcast(spfsMPIMidBcastRequest* msg)
{
    // get parent uni id
    rspUniId_ = msg->getUniId();

    // calculate number of tree levels
    int steps = (int)ceil(log(msg->getRankSetArraySize())/log(2));
    // calculate current level
    int currentStep = msg->getStep() + 1;
    int selfRank = -1;

    rankNum_ = 0;
    // get parent's owner rank
    rspRank_ = msg->getRoot();

    // if not the tree root, send the embedded msg to app
    if(rspRank_ != mpiMiddleware_->getRank())
    {
        cMessage * enmsg = static_cast<cMessage*>(msg->encapsulatedMsg()->dup());
        if(enmsg)mpiMiddleware_->sendApp(enmsg);
    }

    // find self virtual rank in the bcasting virtual rank set
    for(unsigned int i = 0; i < msg->getRankSetArraySize(); i++)
    {
        if(msg->getRankSet(i) == mpiMiddleware_->getRank())selfRank = i;
    }

    // if in the v-rank set
    if(selfRank != -1)
    {
        for(int i = currentStep; i < steps; i ++)
        {
            // calculate next direct child node rank
            unsigned int targetRank = selfRank ^ (0x1 << i);

            // prepare msg for child
            // update step
            msg->setStep(i);
            if(targetRank >= msg->getRankSetArraySize())
            {
                continue;
            }

            // update uni id & subtree root
            msg->setUniId((long)this);
            msg->setRoot(msg->getRankSet(selfRank));
      
            spfsMPISendRequest* req = new spfsMPISendRequest();
            cMessage* dupMsg = static_cast<cMessage*>(msg->dup());

            req->setRank(msg->getRankSet(targetRank));
            req->encapsulate(dupMsg);

            // send msg to child
            mpiMiddleware_->sendNet(req);
            rankNum_ ++;
        }
    }

    delete msg;
}

void MPIMidBcastSM::enterWait(spfsMPIMidBcastResponse* msg)
{ 
    long uniId = (long)this;
    if(uniId == msg->getUniId())
    {
        // count for number of responded nodes
        rspNum_++;
    }
    delete msg;
}

void MPIMidBcastSM::enterRsp()
{
    // if i am the root
    if(rspRank_ == mpiMiddleware_->getRank())
    {
        // may need to response to app
    }
    else
    {
        // response to rspRank_, which is the message sender
        spfsMPIMidBcastResponse *msg = new spfsMPIMidBcastResponse("bcast rsp", SPFS_MPIMID_BCAST_RESPONSE);
        msg->setUniId(rspUniId_);

        spfsMPISendRequest* req = new spfsMPISendRequest();
        req->setRank(rspRank_);
        req->encapsulate(static_cast<cMessage*>(msg));
        mpiMiddleware_->sendNet(req);
    }
    
    // now ready to destory self
    delete this;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
