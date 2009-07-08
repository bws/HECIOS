#ifndef MPI_MIDDLEWARE_BCAST_SM_H
#define MPI_MIDDLEWARE_BCAST_SM_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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

    bool finished() {return finished_;};

    int state() {return currentState_.state();};

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
    bool finished_;
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
