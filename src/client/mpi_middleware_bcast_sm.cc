//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
#include "mpi_middleware_bcast_sm.h"
#include "mpi_middleware.h"
#include "mpi_mid_m.h"
#include "mpi_proto_m.h"
#include "comm_man.h"
#include <iostream>
#include <cassert>

using namespace std;

MPIMidBcastSM::MPIMidBcastSM(MpiMiddleware *mpi_mid)
{
    mpiMiddleware_ = mpi_mid;
    rspCounter_ = 0;
    childNum_ = 0;
    parentWRank_ = -1;
    finished_ = false;
}

void MPIMidBcastSM::handleMessage(cMessage* msg)
{
    enum
    {
        INIT = 0,
        BCAST = FSM_Transient(1),
        WAIT = FSM_Steady(2),
        RSP = FSM_Transient(3),
    };

    FSM_Switch(currentState_)
    {
        case FSM_Enter(INIT):
            {
//                cerr << mpiMiddleware_->rank() << " " << this << "enter INIT" << msg->kind() << endl;
                rspCounter_ = 0;
                childNum_ = 0;
                parentWRank_ = -1;
                finished_ = true;
                break;
            }
        case FSM_Exit(INIT):
            {
//                cerr << mpiMiddleware_->rank() << " " << this << "exit INIT " << msg->kind() << endl;
                if(msg->kind() == SPFS_MPIMID_BCAST_REQUEST)
                    if(spfsMPIMidBcastRequest* req = dynamic_cast<spfsMPIMidBcastRequest*>(msg))
                    {
                        req = 0;
                        FSM_Goto(currentState_, BCAST);
                    }
                break;
            }

        case FSM_Enter(BCAST):
            {
//                cerr << mpiMiddleware_->rank() << "enter BCAST" << msg->kind() << endl;
                if(msg->kind() == SPFS_MPIMID_BCAST_REQUEST)
                    if(spfsMPIMidBcastRequest* req = dynamic_cast<spfsMPIMidBcastRequest*> (msg))
                        enterBcast(req);

                break;
            }

        case FSM_Exit(BCAST):
            {
//                cerr << mpiMiddleware_->rank() << "exit BCAST" << msg->kind() << endl;
                if(childNum_ != 0)
                    FSM_Goto(currentState_, WAIT);
                else
                    FSM_Goto(currentState_, RSP);
                break;
            }

        case FSM_Enter(WAIT):
            {
//                cerr << mpiMiddleware_->rank() << "enter WAIT" << msg->kind() << endl;
                if(msg->kind() == SPFS_MPIMID_BCAST_RESPONSE)
                    if(spfsMPIMidBcastResponse* rsp = dynamic_cast<spfsMPIMidBcastResponse*> (msg))
                        enterWait(rsp);
                break;
            }

        case FSM_Exit(WAIT):
            {
//                cerr << mpiMiddleware_->rank() << "exit WAIT" << rspCounter_ << ":" << childNum_ << endl;
                if(msg->kind() == SPFS_MPIMID_BCAST_RESPONSE)
                    if(rspCounter_ == childNum_ - 1)
                        FSM_Goto(currentState_, RSP);
                break;
            }

        case FSM_Enter(RSP):
            {
//                cerr << mpiMiddleware_->rank() << "enter RSP" << msg->kind() << endl;
                enterRsp();
                break;
            }
        case FSM_Exit(RSP):
            {
//                cerr << mpiMiddleware_->rank() << "exit RSP" << msg->kind() << endl;
                finished_ = true;
                FSM_Goto(currentState_, INIT);
                break;
            }
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "Dear god what went wrong" << endl;
            break;
        }
    }
    delete msg;
}

void MPIMidBcastSM::enterBcast(spfsMPIMidBcastRequest* msg)
{
    int size = CommMan::instance().commSize(msg->getCommunicator());
    int parent = msg->getParent();
    int root = msg->getRoot();
    int rank = CommMan::instance().commRank(msg->getCommunicator(),
                                                mpiMiddleware_->rank());

    parentSM_ = msg->contextPointer();
    // calculate number of tree levels
    int steps = (int)ceil(log(size) / log(2));
    // calculate current level

    int mystep = 0;

    int dist = (size + rank - parent) % (0x1 << steps);
    while(dist != 0)
    {
        dist >>= 1;
        mystep ++;
    }

    if(rank != root)
    {
        // send embed msg to app
        cMessage * enmsg = msg->encapsulatedMsg();
        enmsg = (cMessage*)(enmsg->dup());
        if(enmsg)mpiMiddleware_->sendApp(enmsg);
    }

    parentWRank_ = CommMan::instance().commTrans(msg->getCommunicator(),
                                                     parent, MPI_COMM_WORLD);

    for(int i = mystep; i < steps; i ++)
    {
        // calculate next direct child node rank
        int child_rank = (rank + (0x1 << i)) % (0x1 << steps);
        if(child_rank < size)
        {
            spfsMPISendRequest* req = new spfsMPISendRequest();
            cMessage* dupMsg = static_cast<cMessage*>(msg->dup());

            dynamic_cast<spfsMPIMidBcastRequest*>(dupMsg)->setParent(rank);
            dynamic_cast<spfsMPIMidBcastRequest*>(dupMsg)->setContextPointer(this);

            int child_wrank = CommMan::instance().commTrans(
                msg->getCommunicator(), child_rank, MPI_COMM_WORLD);

            req->setRank(child_wrank);
            req->encapsulate(dupMsg);

            // send msg to child
            mpiMiddleware_->sendNet(req);
            childNum_ ++;
        }
    }

    delete msg->decapsulate();
}

void MPIMidBcastSM::enterWait(spfsMPIMidBcastResponse* msg)
{
    rspCounter_++;
}

void MPIMidBcastSM::enterRsp()
{
    if(parentWRank_ == mpiMiddleware_->rank())
    {
        // response to app
        spfsMPIBcastResponse *msg = new spfsMPIBcastResponse("bcast rsp", SPFS_MPI_BCAST_RESPONSE);
        mpiMiddleware_->sendApp(msg);
    }
    else
    {
        // response to parent
        spfsMPIMidBcastResponse *msg = new spfsMPIMidBcastResponse("bcast rsp", SPFS_MPIMID_BCAST_RESPONSE);

        spfsMPISendRequest* req = new spfsMPISendRequest();

        req->setRank(parentWRank_);
        req->encapsulate(static_cast<cMessage*>(msg));
        msg->setContextPointer(parentSM_);
        mpiMiddleware_->sendNet(req);
    }
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
