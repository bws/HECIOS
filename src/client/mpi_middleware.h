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

class spfsCacheInvalidateRequest;
class spfsMPIMidBcastRequest;
class spfsMPIBcastRequest;
class MPIMidBcastSM;

using namespace std;

/**
 * Model of the MPI middleware
 */
class MpiMiddleware : public cSimpleModule
{
public:
    /** Constructor */
    MpiMiddleware() : cSimpleModule(), rank_(-1) {};

    /** @return The Rank of this node */
    int getRank();

    void setRank(int r){rank_ = r;};

    /** Sends Message to net out gate */
    void sendNet(cMessage *msg);

    /** Sends Message to app out gate */
    void sendApp(cMessage *msg);
  
protected:
    /** Implementation of initialize */
    virtual void initialize();
  
    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    int appInGate_;
    int appOutGate_;
    int netServerInGate_;
    int netClientInGate_;
    int netServerOutGate_;
    int netClientOutGate_;
    int rank_;
    
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
