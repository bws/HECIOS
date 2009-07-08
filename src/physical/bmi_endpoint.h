//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
#include "basic_types.h"
#include "serial_message_scheduler.h"
class spfsBMIExpectedMessage;
class spfsBMIMessage;
class spfsBMIPushDataRequest;
class spfsBMIPushDataResponse;
class spfsBMIUnexpectedMessage;
class spfsRequest;
class spfsResponse;

/**
 * Model of an abstract BMI endpoint
 */
class BMIEndpoint : public cSimpleModule
{
public:
    /** Number of bytes required as overhead to send a BMI unexpected msg */
    static const unsigned int BMI_UNEXPECTED_MSG_BYTES = 24;

    /** Number of bytes required as overhead to send a BMI expected response */
    static const unsigned int BMI_EXPECTED_MSG_BYTES = 0;

    /** Constructor */
    BMIEndpoint();

    /** Set the enpoints shandle range to provide service to */
    void setHandleRange(const HandleRange& handleRange);

    /** @return a BMIExpected message encapsulating msg */
    virtual spfsBMIExpectedMessage* createExpectedMessage(cMessage* msg) = 0;

    /** @return a BMIUnexpected message encapsulating msg */
    virtual spfsBMIUnexpectedMessage* createUnexpectedMessage(
        spfsRequest* request) = 0;

protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of cSimpleModule::handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Initialize derived endpoint implementation */
    virtual void initializeEndpoint() = 0;

    /** Finalize derived endpoint implementation*/
    virtual void finalizeEndpoint() = 0;

    /** Extract the domain message from a BMI message */
    virtual cMessage* extractBMIPayload(spfsBMIMessage* bmiMsg);

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIExpectedMessage* expectedMsg) = 0;

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIUnexpectedMessage* unexpectedMsg) = 0;

private:
    /** handle messages received from the network */
    void handleMessageFromNetwork(cMessage* msg);

    /** @return true if handle corresponds to this node */
    bool handleIsLocal(const FSHandle& handle);

    /** @return the delay before the inbound message can be delivered */
    simtime_t getNextMessageInScheduleTime(size_t byteLength);

    /** @return the delay before the outbound message can be delivered */
    simtime_t getNextMessageOutScheduleTime(size_t byteLength);

    /** Gate id for appIn */
    int appInGateId_;

    /** Gate id for appOut */
    int appOutGateId_;

    /** Handle range for this endpoint */
    HandleRange handleRange_;

    /** Variable for serializing all inbound messages */
    SerialMessageScheduler messageInScheduler_;
    double nextMessageScheduledInTime_;

    /** Variable for serializing all outbound messages */
    SerialMessageScheduler messageOutScheduler_;
    double nextMessageScheduledOutTime_;

    /** Fixed overhead for sending and receiving a BMI message */
    double fixedOverheadSecs_;

    /** Data size dependent overhead for sending and receiving a BMI message */
    double scaledOverheadSecs_;
};

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
