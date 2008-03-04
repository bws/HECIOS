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
    
    /** @return a BMIExpected message encapsulating msg */
    virtual spfsBMIExpectedMessage* createExpectedMessage(cMessage* msg);
    
    /** @return a BMIUnexpected message encapsulating msg */
    virtual spfsBMIUnexpectedMessage* createUnexpectedMessage(
        spfsRequest* request);
    
protected:
    /** Initialize this enpoint type */
    virtual void initializeEndpoint();

    /** Finalize this endpoint type */
    virtual void finalizeEndpoint();

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIExpectedMessage* expectedMsg);
    
    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIUnexpectedMessage* unexpectedMsg);

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

spfsBMIUnexpectedMessage* BMIDirectEndpoint::createUnexpectedMessage(
    spfsRequest* request)
{
    assert(0 != request);
    spfsBMIUnexpectedMessage* pkt = new spfsBMIUnexpectedMessage();
    pkt->setHandle(request->getHandle());
    pkt->setByteLength(BMI_UNEXPECTED_MSG_BYTES + DIRECT_OVERHEAD_BYTES);
    pkt->encapsulate(request);
    return pkt;
}

spfsBMIExpectedMessage* BMIDirectEndpoint::createExpectedMessage(
    cMessage* msg)
{
    assert(0 != msg);

    spfsBMIExpectedMessage* pkt = new spfsBMIExpectedMessage();
    pkt->setByteLength(BMI_EXPECTED_MSG_BYTES + DIRECT_OVERHEAD_BYTES);
    pkt->encapsulate(msg);
    return pkt;
}

void BMIDirectEndpoint::sendOverNetwork(spfsBMIExpectedMessage* msg)
{
    assert(0 != msg);
    send(msg, netOutGateId_);
}

void BMIDirectEndpoint::sendOverNetwork(spfsBMIUnexpectedMessage* msg)
{
    assert(0 != msg);
    send(msg, netOutGateId_);
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
