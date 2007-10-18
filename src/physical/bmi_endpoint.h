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
class spfsBMIMessage;
class spfsRequest;
class spfsResponse;

/**
 * Model of an abstract BMI endpoint
 */
class BMIEndpoint : public cSimpleModule
{
public:
    /** Number of bytes required as overhead to use the BMI protocol */
    static const unsigned int BMI_OVERHEAD_BYTES = 4;
    
    /** Constructor */
    BMIEndpoint() : cSimpleModule() {};

    /** Set the enpoints shandle range to provide service to */
    void setHandleRange(const HandleRange& handleRange);
    
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

    /** Send msg as a BMIExpectedMessage over the network */
    virtual void sendBMIExpectedMessage(cMessage* msg) = 0;

    /** Send msg as a BMIUnexpectedMessage over the network */
    virtual void sendBMIUnexpectedMessage(spfsRequest* request) = 0;

    /** Extract the domain message from a BMI message */
    virtual cMessage* extractBMIPayload(spfsBMIMessage* bmiMsg) = 0;
    
private:
    /** @return true if handle corresponds to this node */
    bool handleIsLocal(const FSHandle& handle);
    
    /** Gate id for appIn */
    int appInGateId_;

    /** Gate id for appOut */
    int appOutGateId_;

    /** Handle range for this endpoint */
    HandleRange handleRange_;
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
