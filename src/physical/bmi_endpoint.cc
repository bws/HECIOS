//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "bmi_endpoint.h"
#include <cassert>
#include <climits>
#include <iostream>
#include <omnetpp.h>
#include "bmi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

BMIEndpoint::BMIEndpoint()
    : cSimpleModule(),
      messageInScheduler_(this),
      messageOutScheduler_(this)
{
}
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

    // Extract the BMI overhead costs
    fixedOverheadSecs_ = par("fixedOverheadSecs").doubleValue();
    scaledOverheadSecs_ = par("scaledOverheadSecs").doubleValue();

    // Initialize the in and outbound message shedule times
    nextMessageScheduledInTime_ = 0.0;
    nextMessageScheduledOutTime_ = 0.0;

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
    // If the request is from the application,
    //     reschedule it to account for fixed network costs
    // else if its a self message
    //     send it over the network
    // else
    //     its from the network, send it to the application/server
    if (msg->getArrivalGateId() == appInGateId_)
    {
        // Perform the network queueing costs
        //TODO: This used to use the message size
        simtime_t scheduleTime =
            getNextMessageOutScheduleTime(0);
        scheduleAt(scheduleTime, msg);
    }
    else if (msg->isSelfMessage())
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
            cerr << __FILE__ << ":" << __LINE__ << ": "
                 << "Unexpected messages should not be sent directly.\n";
            assert(false);
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

    // Calculate the network queueing costs
    simtime_t currentTime = simulation.getSimTime();
    //TODO: This used to use the actual message size
    simtime_t delay = getNextMessageInScheduleTime(0) - currentTime;

    // If the message is a flow message, send it directly
    // Otherwise extract the payload and send it on
    if (0 != dynamic_cast<spfsBMIPushDataRequest*>(msg) ||
        0 != dynamic_cast<spfsBMIPushDataResponse*>(msg))
    {
        sendDelayed(msg, delay, appOutGateId_);
    }
    else
    {
        spfsBMIMessage* bmiMsg = dynamic_cast<spfsBMIMessage*>(msg);
        assert(0 != bmiMsg);
        cMessage* pfsMsg =  extractBMIPayload(bmiMsg);
        sendDelayed(pfsMsg, delay, appOutGateId_);
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

bool BMIEndpoint::handleIsLocal(const FSHandle& handle)
{
    return (handle >= handleRange_.first && handle <= handleRange_.last);
}

simtime_t BMIEndpoint::getNextMessageInScheduleTime(size_t byteLength)
{
    simtime_t delay = fixedOverheadSecs_ + (scaledOverheadSecs_ *
                                            byteLength);
    return messageInScheduler_.getNextMessageScheduleTime(delay);
}

simtime_t BMIEndpoint::getNextMessageOutScheduleTime(size_t byteLength)
{
    simtime_t delay = fixedOverheadSecs_ + (scaledOverheadSecs_ *
                                            byteLength);
    return messageOutScheduler_.getNextMessageScheduleTime(delay);
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
