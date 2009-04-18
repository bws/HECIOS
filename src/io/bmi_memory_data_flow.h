#ifndef BMI_MEMORY_DATA_FLOW_H
#define BMI_MEMORY_DATA_FLOW_H
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
#include "data_flow.h"
#include "filename.h"
class spfsDataFlowStart;
class spfsBMIPushDataRequest;
class cSimpleModule;
class cMessage;

/** A data flow transferring data between memory and BMI*/
class BMIMemoryDataFlow : public DataFlow
{
public:
    /** BMI-to-Memory data flow constructor */
    BMIMemoryDataFlow(const spfsDataFlowStart& flowStart,
                      std::size_t numBuffers,
                      FSSize bufferSize,
                      cSimpleModule* module);

    /** Destructor */
    virtual ~BMIMemoryDataFlow();

protected:
    /** Start the flow of data */
    virtual void startTransfer();

    /** Process BMI and Storage request and responses */
    virtual void processDataFlowMessage(cMessage* msg);

    /** Use standard BMI calls to send data over the network */
    virtual void pushDataToNetwork(FSSize pushSize);

    /** Use standard BMI calls to request data from the network */
    virtual void pullDataFromNetwork(FSSize pullSize);

    /** Use the ListIO interface to write data to disk */
    virtual void pushDataToStorage(FSSize pushSize);

    /** Use the ListIO interface to read data from disk */
    virtual void pullDataFromStorage(FSSize pullSize);

private:
    /** Hidden copy constructor */
    BMIMemoryDataFlow(const BMIMemoryDataFlow& other);

    /** Hidden assignment operator */
    BMIMemoryDataFlow& operator=(const BMIMemoryDataFlow& other);

    /** Send the BMI data receipt acknowledgment */
    void sendPushAck(spfsBMIPushDataRequest* request);

    /** File used for I/O */
    Filename filename_;

    /** The module to send data over */
    cSimpleModule* module_;

    /** The BMI connection id */
    int bmiConnectionId_;

    /** The BMI tag used to send outbound data */
    int outboundBmiTag_;

    /** The amount of data pushed into the network */
    FSSize amountPushed_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
