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
#include <omnetpp.h>
#include "basic_types.h"
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
    BMIEndpoint() : cSimpleModule() {};

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
    
    /** Gate id for appIn */
    int appInGateId_;

    /** Gate id for appOut */
    int appOutGateId_;

    /** Handle range for this endpoint */
    HandleRange handleRange_;

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
