#ifndef MPI_MIDDLEWARE_H
#define MPI_MIDDLEWARE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
#include "mpi_communication_helper.h"
class spfsCacheInvalidateRequest;
class spfsMPIBarrierRequest;
class spfsMPIBarrierResponse;
class spfsMPIBcastRequest;
class spfsMPIBcastResponse;
class spfsMPIRequest;
using namespace std;

/**
 * Model of the MPI middleware
 */
class MpiMiddleware : public cSimpleModule, MPICommunicationUserIF
{
public:
    /** Constructor */
    MpiMiddleware();

    /** Set the rank for the middleware */
    void setRank(int r);

    /** @return the rank of this node */
    int rank() const;

    /** Callback for when a communication completes */
    void completeCommunicationCB(spfsMPIRequest* request);

protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    /** @return the completion response for the barrier */
    spfsMPIBarrierResponse* createBarrierResponse(
        spfsMPIBarrierRequest* request) const;

    /** @return the completion response for the bcast */
    spfsMPIBcastResponse* createBcastResponse(
        spfsMPIBcastRequest* request) const;

    /** Gate to recv messages from application on */
    int appInGate_;

    /** Gate to send messages to the application on */
    int appOutGate_;

    /** Cache communication gates */
    int cacheInGate_;
    int cacheOutGate_;

    /** Network communication gates */
    int netInGate_;
    int netOutGate_;

    /** Process rank for this MPI middleware */
    int rank_;

    /** The mean of the random delay from middleware messaging */
    double randomDelayMean_;
};


#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
