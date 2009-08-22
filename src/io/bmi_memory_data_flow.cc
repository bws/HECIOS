//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "bmi_memory_data_flow.h"
#include <cassert>
#include <vector>
#include "bmi_proto_m.h"
#include "data_flow_registry.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

BMIMemoryDataFlow::BMIMemoryDataFlow(const spfsDataFlowStart& flowStart,
                                     std::size_t numBuffers,
                                     FSSize bufferSize,
                                     cSimpleModule* module)
    : DataFlow(flowStart, numBuffers, bufferSize, module),
      filename_(flowStart.getHandle()),
      module_(module),
      bmiConnectionId_(flowStart.getBmiConnectionId()),
      outboundBmiTag_(flowStart.getOutboundBmiTag()),
      amountPushed_(0)
{
}

BMIMemoryDataFlow::~BMIMemoryDataFlow()
{
}

void BMIMemoryDataFlow::startTransfer()
{
    assert(INVALID_MODE != getMode());
    if (WRITE_MODE == getMode())
    {
        FSSize bufSize = min(getBufferSize(), getSize() - amountPushed_);
        if (0 < bufSize)
        {
            pushDataToNetwork(bufSize);
        }
    }
    else
    {
        // No-op, just wait for data arrival
    }
}

void BMIMemoryDataFlow::processDataFlowMessage(cMessage* msg)
{
    if (0 != dynamic_cast<spfsBMIPushDataRequest*>(msg))
    {
        spfsBMIPushDataRequest* pushRequest =
            static_cast<spfsBMIPushDataRequest*>(msg);
        sendPushAck(pushRequest);
        addNetworkProgress(pushRequest->getDataSize());
        addStorageProgress(pushRequest->getDataSize());

        // Perform data collection for flow progress
        collectTransferFromNetworkDelay(pushRequest);
    }
    else if (0 != dynamic_cast<spfsBMIPushDataResponse*>(msg))
    {
        spfsBMIPushDataResponse* pushResp =
            static_cast<spfsBMIPushDataResponse*>(msg);
        addNetworkProgress(pushResp->getReceivedSize());
        addStorageProgress(pushResp->getReceivedSize());

        // Perform data collection for flow progress
        collectTransferToNetworkDelay(pushResp);

        // If data remains to be sent, send it
        FSSize bufSize = min(getBufferSize(), getSize() - amountPushed_);
        if (0 != bufSize)
        {
            pushDataToNetwork(bufSize);
        }
    }

    // If the flow is complete, send the finish message
    if (isComplete())
    {
        // Send the final response
        spfsDataFlowFinish* flowFinish =
            new spfsDataFlowFinish(0, SPFS_DATA_FLOW_FINISH);
        flowFinish->setContextPointer(getOriginatingMessage());
        flowFinish->setFlowId(getUniqueId());
        module_->scheduleAt(simulation.getSimTime(), flowFinish);
        //cerr << "Finishing BMI-Memory flow\n";
    }

    // Cleanup the message
    delete msg;
}

void BMIMemoryDataFlow::pullDataFromNetwork(FSSize pullSize)
{
    assert(false);
}

void BMIMemoryDataFlow::pushDataToNetwork(FSSize pushSize)
{
    assert(0 < pushSize);
    spfsBMIPushDataRequest* pushRequest = new spfsBMIPushDataRequest();
    spfsDataFlowStart* startMsg = getOriginatingMessage();
    pushRequest->setContextPointer(startMsg);
    pushRequest->setConnectionId(bmiConnectionId_);
    pushRequest->setFlowId(getUniqueId());
    pushRequest->setTag(outboundBmiTag_);
    pushRequest->setFlowSize(getSize());
    pushRequest->setDataSize(pushSize);
    pushRequest->setByteLength(pushSize);

    // Send the push request
    //cerr << "Pushing client data: " << pushSize << endl;
    module_->send(pushRequest, "netOut");
    amountPushed_ += pushSize;
}

void BMIMemoryDataFlow::pullDataFromStorage(FSSize pullSize)
{
    assert(false);
}

void BMIMemoryDataFlow::pushDataToStorage(FSSize pushSize)
{
    assert(false);
}

void BMIMemoryDataFlow::sendPushAck(spfsBMIPushDataRequest* request)
{
    // Send the push acknowedgement, allowing the sender to continue
    spfsBMIPushDataResponse* pushResponse = new spfsBMIPushDataResponse();
    spfsDataFlowStart* startMsg = getOriginatingMessage();
    pushResponse->setContextPointer(startMsg);
    pushResponse->setConnectionId(bmiConnectionId_);
    pushResponse->setFlowId(getUniqueId());
    pushResponse->setTag(outboundBmiTag_);
    pushResponse->setReceivedSize(request->getDataSize());
    pushResponse->setByteLength(0);

    // Look up the partner flow to send a direct out-of-band message
    DataFlow* partner =
        DataFlowRegistry::instance().getSubscribedDataFlow(outboundBmiTag_);
    assert(0 != partner);
    cModule* partnerModule = partner->parentModule();
    assert(0 != partnerModule);

    // Send the acknowledgment directly to module
    parentModule()->sendDirect(pushResponse, 0.0, 0.0, partnerModule, "directIn");
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
