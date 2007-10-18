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
#include <cassert>
#include <iostream>
#include <map>
#include "bmi_endpoint.h"
#include "bmi_proto_m.h"
#include "pvfs_proto_m.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a bmi endpoint that uses direct messaging
 */
class BMIDirectEndpoint : public BMIEndpoint
{
public:
    /** Number of bytes required as overhead to use the BMI direct network */
    static const unsigned int DIRECT_OVERHEAD_BYTES = 0;
    
protected:
    /** Initialize this enpoint type */
    virtual void initializeEndpoint();

    /** Finalize this endpoint type */
    virtual void finalizeEndpoint();

    /** Send a BMI message with an established recipient */
    virtual void sendBMIExpectedMessage(cMessage* msg);

    /** Send a BMI message to a recipient not expecting a message */
    virtual void sendBMIUnexpectedMessage(spfsRequest* request);
    
    /** Extract the domain message from a BMI message */
    virtual cMessage* extractBMIPayload(spfsBMIMessage* bmiMsg);

private:
    /** Gate id for netIn */
    int netInGateId_;

    /** Gate id for netOut */
    int netOutGateId_;
};

// OMNet Registriation Method
Define_Module(BMIDirectEndpoint);

void BMIDirectEndpoint::initializeEndpoint()
{
    // Extract the gate ids
    netInGateId_ = findGate("netIn");
    netOutGateId_ = findGate("netOut");
}

void BMIDirectEndpoint::finalizeEndpoint()
{
}

void BMIDirectEndpoint::sendBMIExpectedMessage(cMessage* msg)
{
    assert(0 != msg);
    spfsBMIExpectedMessage* pkt = new spfsBMIExpectedMessage();
    pkt->setByteLength(DIRECT_OVERHEAD_BYTES);
    pkt->encapsulate(msg);
    send(pkt, netOutGateId_);
}

void BMIDirectEndpoint::sendBMIUnexpectedMessage(spfsRequest* request)
{
    assert(0 != request);
    spfsBMIUnexpectedMessage* pkt = new spfsBMIUnexpectedMessage();
    pkt->setByteLength(DIRECT_OVERHEAD_BYTES);
    pkt->encapsulate(request);
    send(pkt, netOutGateId_);
}

cMessage* BMIDirectEndpoint::extractBMIPayload(spfsBMIMessage* bmiMsg)
{
    assert(0 != bmiMsg);
    cMessage* payload = bmiMsg->decapsulate();
    return payload;
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
