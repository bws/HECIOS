#ifndef MPI_MIDDLEWARE_BCAST_SM_H
#define MPI_MIDDLEWARE_BCAST_SM_H
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

/**
 * State Machine Handling MPI middleware bcast req/rsp
 * States:
 *        +------------+   rsp    +------------+ rsp=num? +----------+
 * INIT ->|  Bcast     | -------> | Wait 4 Rsp | -------> | Response |-> DONE
 *        +------------+          +------------+    |     +----------+
 *              | no child               ^----------+ no       ^
 *              +----------------------------------------------|
 */

class MpiMiddleware;
class spfsMPIMidBcastRequest;
class spfsMPIMidBcastResponse;
class cFSM;

/** mpi middleware bcast state machine */
class MPIMidBcastSM
{
public:
    /** Constructor */
    MPIMidBcastSM(MpiMiddleware *mpi_mid);
  
    /** Handle MPI-mid-bcast Msg */
    void handleMessage(cMessage* msg);

    ~MPIMidBcastSM() {};

protected:
    /** enter bcast state */
    void enterBcast(spfsMPIMidBcastRequest* msg);

    /** enter wait state */
    void enterWait(spfsMPIMidBcastResponse* msg);

    /** enter response state */
    void enterRsp();

private:
    MpiMiddleware *mpiMiddleware_; // point to owner class
    cFSM currentState_; // finite state machine
    int rspCounter_;
    int childNum_;
    int parentWRank_;
    void *parentSM_;
};


#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
