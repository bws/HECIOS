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
#include <climits>
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
    HandleRange init = {UINT_MAX, UINT_MAX - 1};
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
    // If the request is from the application, send it over the network
    // else its from the network, send it to the application/server
    if (msg->arrivalGateId() == appInGateId_)
    {
        // Handle requests, responses, and data flows seperately
        if (spfsRequest* req = dynamic_cast<spfsRequest*>(msg))
        {
            spfsBMIUnexpectedMessage* bmiMsg = createUnexpectedMessage(req);
            sendOverNetwork(bmiMsg);
        }
        else if (spfsResponse* resp = dynamic_cast<spfsResponse*>(msg))
        {
            spfsBMIExpectedMessage* expectedMsg = createExpectedMessage(resp);
            sendOverNetwork(expectedMsg);
        }
        else if (spfsBMIExpectedMessage* expected =
                 dynamic_cast<spfsBMIExpectedMessage*>(msg))
        {
            sendOverNetwork(expected);
        }        
        else
        {
            spfsBMIUnexpectedMessage* unexpected =
                dynamic_cast<spfsBMIUnexpectedMessage*>(msg);
            assert(0 != msg);
            sendOverNetwork(unexpected);
        }
    }
    else
    {
        handleMessageFromNetwork(msg);
    }
}

void BMIEndpoint::handleMessageFromNetwork(cMessage* msg)
{
    assert(0 != msg);
    
    // If the message is a pull or push request, create a response
    // Else if the message is a push or pull response, fwd to the app
    // Otherwise, extract the PFS payload, and send that to the app
    if (spfsBMIPullDataRequest* pullReq =
        dynamic_cast<spfsBMIPullDataRequest*>(msg))
    {
        pullDataRequestReceived(pullReq);
    }
    else if (spfsBMIPushDataRequest* pushReq =
             dynamic_cast<spfsBMIPushDataRequest*>(msg))
    {
        pushDataRequestReceived(pushReq);
    }
    else if (0 != dynamic_cast<spfsBMIPullDataResponse*>(msg) ||
             0 != dynamic_cast<spfsBMIPushDataResponse*>(msg))
    {
        send(msg, appOutGateId_);
    }
    else
    {
        spfsBMIMessage* bmiMsg = dynamic_cast<spfsBMIMessage*>(msg);
        assert(0 != bmiMsg);
        cMessage* pfsMsg =  extractBMIPayload(bmiMsg);
        send(pfsMsg, appOutGateId_);
        delete msg;        
    }
}

cMessage* BMIEndpoint::extractBMIPayload(spfsBMIMessage* bmiMsg)
{
    assert(0 != bmiMsg);
    
    // Decapsulate the payload
    cMessage* payload = bmiMsg->decapsulate();
    assert(0 != payload);
    return payload;
}

void BMIEndpoint::pullDataRequestReceived(spfsBMIPullDataRequest* pullRequest)
{
    spfsBMIPullDataResponse* pullResp = createPullDataResponse(pullRequest);
    sendOverNetwork(pullResp);
}

void BMIEndpoint::pushDataRequestReceived(spfsBMIPushDataRequest* pushRequest)
{
    spfsBMIPushDataResponse* pushResp = createPushDataResponse(pushRequest);
    sendOverNetwork(pushResp);
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
