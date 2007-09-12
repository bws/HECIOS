//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
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
#include <iostream>
#include <omnetpp.h>
using namespace std;

/**
 * Model of a middleware file system data cache.
 */
class NoMiddlewareCache : public cSimpleModule
{
public:
    /** Constructor */
    NoMiddlewareCache();
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    int appInGateId_;
    int fsInGateId_;
};

// OMNet Registriation Method
Define_Module(NoMiddlewareCache);

NoMiddlewareCache::NoMiddlewareCache()
{
}

void NoMiddlewareCache::initialize()
{
    appInGateId_ = gate("appIn")->id();
    fsInGateId_ = gate("fsIn")->id();
}

/**
 * Handle MPI-IO Response messages
 */
void NoMiddlewareCache::handleMessage(cMessage* msg)
{
    if (msg->arrivalGateId() == appInGateId_)
    {
        send(msg, "fsOut");
    }
    else if (msg->arrivalGateId() == fsInGateId_)
    {
        send(msg, "appOut");
    }
    else
    {
            cerr << "MiddlewareCache handleMessage: "
                 << "not yet implemented for kind: "
                 << msg->kind() << endl;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
