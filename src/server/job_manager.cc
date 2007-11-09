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
#include "job_manager.h"
#include <cassert>
#include "basic_types.h"
#include "bmi_list_io_data_flow.h"
#include "bmi_proto_m.h"
#include "data_flow.h"
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
    netInGateId_ = findGate("netIn");
    netOutGateId_ = findGate("netOut");
    pfsInGateId_ = findGate("pfsIn");
    pfsOutGateId_ = findGate("pfsOut");
    storageInGateId_ = findGate("storageIn");
    storageOutGateId_ = findGate("storageOut");
}

void JobManager::finish()
{
}

int JobManager::registerDataFlow(DataFlow* flow)
{
    assert(0 != flow);
    int uid = flow->getUniqueId();
    dataFlowsById_[uid] = flow;
    return uid;
}

void JobManager::deregisterDataFlow(int flowId)
{
    assert(0 != lookupDataFlow(flowId));
    dataFlowsById_.erase(flowId);
}

DataFlow* JobManager::lookupDataFlow(int flowId) const
{
    DataFlow* flow = 0;
    map<int, DataFlow*>::const_iterator pos = dataFlowsById_.find(flowId);
    if (dataFlowsById_.end() != pos)
    {
        flow = pos->second;
    }
    return flow;
}

void JobManager::handleMessage(cMessage* msg)
{
    // Classify the message by its arrival gate
    if (msg->isSelfMessage())
    {
        handleSelfMessage(msg);
    }
    else if (msg->arrivalGateId() == netInGateId_)
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
    // Cleanup the flow job resources
    spfsDataFlowFinish* flowFinish = dynamic_cast<spfsDataFlowFinish*>(msg);
    assert(0 != flowFinish);
    int flowId = flowFinish->getFlowId();
    DataFlow* flow = lookupDataFlow(flowId);
    assert(0 != flow);
    deregisterDataFlow(flowId);
    delete flow;

    // Send the finish response to the pfs server
    send(msg, pfsOutGateId_);
}

void JobManager::handleNetworkMessage(cMessage* msg)
{
    assert(0 != msg);

    // Route message to the pfs or a flow depending on its message type
    if (spfsBMIFlowMessage* flowMsg = dynamic_cast<spfsBMIFlowMessage*>(msg))
    {
        int flowId = flowMsg->getFlowId();
        DataFlow* flow = lookupDataFlow(flowId);
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
        DataFlow* flow = new BMIListIODataFlow(*flowStart,
                                               flowBuffers_,
                                               flowBufferSize_,
                                               this);

        // Register the flow
        int flowId = registerDataFlow(flow);
        flowStart->setFlowId(flowId);
        flow->initialize();
    }
    else if (0 != dynamic_cast<spfsResponse*>(msg))
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
