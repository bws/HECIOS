#ifndef MPI_MIDDLEWARE_H
#define MPI_MIDDLEWARE_H
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
#include "mpi_communication_helper.h"
class spfsCacheInvalidateRequest;
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

    /** Sends Message to net out gate */
    void sendNet(cMessage *msg);

    /** Sends Message to app out gate */
    void sendApp(cMessage *msg);

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
    /** @return the completion response for the bcast */
    spfsMPIBcastResponse* createBcastResponse(
        spfsMPIBcastRequest* request) const;
    
    /** Gate to recv messages from application on */
    int appInGate_;

    /** Gate to send messages to the application on */
    int appOutGate_;

    /** Network communication gates */
    int netServerInGate_;
    int netClientInGate_;
    int netServerOutGate_;
    int netClientOutGate_;

    /** Process rank for this MPI middleware */
    int rank_;
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
