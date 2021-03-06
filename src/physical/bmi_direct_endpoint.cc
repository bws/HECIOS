//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
    virtual spfsBMIExpectedMessage* createExpectedMessage(cPacket* msg);

    /** @return a BMIUnexpected message encapsulating msg */
    virtual spfsBMIUnexpectedMessage* createUnexpectedMessage(
        spfsRequest* request);

protected:
    /** Initialize this endpoint type */
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
    cPacket* msg)
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
