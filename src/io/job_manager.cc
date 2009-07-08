//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "job_manager.h"
#include <cassert>
#include "basic_types.h"
#include "bmi_list_io_data_flow.h"
#include "bmi_memory_data_flow.h"
#include "bmi_proto_m.h"
#include "data_flow.h"
#include "data_flow_registry.h"
#include "pvfs_proto_m.h"
using namespace std;

// OMNet Registration Method
Define_Module(JobManager);

void JobManager::initialize()
{
    // Read parameters
    flowBufferSize_ = par("bufferSizeInBytes").longValue();
    flowBuffers_ = par("numBuffers");

    // Retrieve gate ids
    directInGateId_ = findGate("directIn");
    netInGateId_ = findGate("netIn");
    netOutGateId_ = findGate("netOut");
    pfsInGateId_ = findGate("pfsIn");
    pfsOutGateId_ = findGate("pfsOut");
    storageInGateId_ = findGate("storageIn");
    storageOutGateId_ = findGate("storageOut");

    // Initialize collection statistics
    totalFlowNetworkBytes_ = 0.0;
    totalFlowStorageBytes_ = 0.0;
}

void JobManager::finish()
{
    recordScalar("SPFS Flow Network Total", totalFlowNetworkBytes_);
    recordScalar("SPFS Flow Storage Total", totalFlowStorageBytes_);
}

DataFlow* JobManager::createDataFlow(spfsDataFlowStart* flowStart)
{
    DataFlow* flow = 0;
    if (DataFlow::SERVER_FLOW_TYPE == flowStart->getFlowType())
    {
        flow = new BMIListIODataFlow(*flowStart,
                                     flowBuffers_,
                                     flowBufferSize_,
                                     this);
    }
    else if (DataFlow::CACHE_FLOW_TYPE == flowStart->getFlowType() ||
             DataFlow::CLIENT_FLOW_TYPE == flowStart->getFlowType())
    {
        flow = new BMIMemoryDataFlow(*flowStart,
                                     flowBuffers_,
                                     flowBufferSize_,
                                     this);
    }
    return flow;
}

int JobManager::registerDataFlow(DataFlow* flow)
{
    return DataFlowRegistry::instance().registerDataFlow(flow);
}

void JobManager::deregisterDataFlow(int flowId)
{
    DataFlowRegistry::instance().deregisterDataFlow(flowId);
}

DataFlow* JobManager::lookupDataFlow(int flowId) const
{
    return DataFlowRegistry::instance().lookupDataFlow(flowId);
}

void JobManager::subscribeDataFlowToTag(DataFlow* flow, int flowTag)
{
    DataFlowRegistry::instance().subscribeDataFlowToTag(flow, flowTag);
}

DataFlow* JobManager::getSubscribedDataFlow(int flowTag) const
{
    return DataFlowRegistry::instance().getSubscribedDataFlow(flowTag);
}

void JobManager::removeSubscriptionTag(int flowTag)
{
    DataFlowRegistry::instance().removeSubscriptionTag(flowTag);
}

void JobManager::unsubscribeDataFlow(DataFlow* flow)
{
    DataFlowRegistry::instance().unsubscribeDataFlow(flow);
}

void JobManager::handleMessage(cMessage* msg)
{
    // Classify the message by its arrival gate
    // cerr << name() << "Message recvd" << endl;
    if (msg->isSelfMessage())
    {
        handleSelfMessage(msg);
    }
    else if (msg->arrivalGateId() == netInGateId_ ||
             msg->arrivalGateId() == directInGateId_)
    {
        handleNetworkMessage(msg);
    }
    else if (msg->arrivalGateId() == pfsInGateId_)
    {
        handlePFSMessage(msg);
    }
    else
    {
        assert(msg->arrivalGateId() == storageInGateId_);
        handleStorageMessage(msg);
    }
}

void JobManager::handleSelfMessage(cMessage* msg)
{
    // Locate the flow for this flow finish message
    spfsDataFlowFinish* flowFinish = dynamic_cast<spfsDataFlowFinish*>(msg);
    assert(0 != flowFinish);
    int flowId = flowFinish->getFlowId();
    DataFlow* flow = lookupDataFlow(flowId);
    assert(0 != flow);

    // Collect statistics
    totalFlowNetworkBytes_ += flow->getNetworkProgress();
    totalFlowStorageBytes_ += flow->getStorageProgress();

    // Cleanup the flow job resources
    //unsubscribeDataFlow(flow);
    deregisterDataFlow(flowId);
    delete flow;

    // Send the finish response to the pfs server
    send(msg, pfsOutGateId_);
}

void JobManager::handleNetworkMessage(cMessage* msg)
{
    assert(0 != msg);
    //cerr << name() << ": Received network message\n";
    // Route message to the pfs or a flow depending on its message type
    if (spfsBMIFlowMessage* flowMsg = dynamic_cast<spfsBMIFlowMessage*>(msg))
    {
        int bmiTag = flowMsg->getTag();
        DataFlow* flow = getSubscribedDataFlow(bmiTag);
        assert(0 != flow);
        flow->handleServerMessage(flowMsg);
    }
    else
    {
        send(msg, pfsOutGateId_);
    }
}

void JobManager::handlePFSMessage(cMessage* msg)
{
    // Determine if the message starts a flow, or should be routed to
    // either the storage or network subssytems
    if (spfsDataFlowStart* flowStart = dynamic_cast<spfsDataFlowStart*>(msg))
    {
        // Create the flow
        DataFlow* flow = createDataFlow(flowStart);
        assert(0 != flow);

        // Register the flow
        int flowId = registerDataFlow(flow);
        flowStart->setFlowId(flowId);

        // Subscribe the flow to a BMI tag
        subscribeDataFlowToTag(flow, flowStart->getInboundBmiTag());

        // Start the flow
        flow->initialize();
    }
    else if (0 != dynamic_cast<spfsResponse*>(msg) ||
             0 != dynamic_cast<spfsRequest*>(msg))
    {
        send(msg, netOutGateId_);
    }
    else
    {
        send(msg, storageOutGateId_);
    }
}

void JobManager::handleStorageMessage(cMessage* msg)
{
    assert(0 != msg);
    assert(0 != msg->contextPointer());

    // Route message to the flow or PFS depending on its originator
    cMessage* request = static_cast<cMessage*>(msg->contextPointer());
    cMessage* origReq = static_cast<cMessage*>(request->contextPointer());
    if (spfsDataFlowStart* flowStart = dynamic_cast<spfsDataFlowStart*>(origReq))
    {
        int flowId = flowStart->getFlowId();
        DataFlow* flow = lookupDataFlow(flowId);
        assert(0 != flow);
        flow->handleServerMessage(msg);
    }
    else
    {
        send(msg, pfsOutGateId_);
    }
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
