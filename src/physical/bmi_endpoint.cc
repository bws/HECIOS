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
#include "bmi_endpoint.h"
#include <cassert>
#include <iostream>
#include <omnetpp.h>
#include "bmi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

void BMIEndpoint::setHandleRange(const HandleRange& handleRange)
{
    handleRange_ = handleRange;
}

void BMIEndpoint::initialize()
{
    // Extract the gate ids
    appInGateId_ = findGate("appIn");
    appOutGateId_ = findGate("appOut");

    // Set the handle range to an invalid range
    HandleRange init = {-1, -1};
    setHandleRange(init);
    
    // Initialize the derived implementation
    initializeEndpoint();
}

void BMIEndpoint::finish()
{
    // Finalize derived implementations
    finalizeEndpoint();
}

void BMIEndpoint::handleMessage(cMessage* msg)
{
    // cerr << "receiving message: " << msg->className() << endl;
    // If the request is from the application, send it over the network
    // else its from the network, send it to the application/server
    if (msg->arrivalGateId() == appInGateId_)
    {
        // Handle requests and responses seperately
        if (spfsRequest* req = dynamic_cast<spfsRequest*>(msg))
        {
            // cerr << "Sending as unexpected: " << req->className() << endl;
            sendBMIUnexpectedMessage(req);        
        }
        else
        {
            //cerr << "Sending as expected: " << msg->className() << endl;
            sendBMIExpectedMessage(msg);
        }
    }
    else
    {
        spfsBMIMessage* bmiMsg = dynamic_cast<spfsBMIMessage*>(msg);
        assert(0 != bmiMsg);
        cMessage* pfsMsg =  extractBMIPayload(bmiMsg);
        //cerr << "Sending over appOut: " << pfsMsg->className() << endl;
        send(pfsMsg, appOutGateId_);
        delete msg;
    }
}

bool BMIEndpoint::handleIsLocal(const FSHandle& handle)
{
    return (handle >= handleRange_.first && handle <= handleRange_.last);
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
