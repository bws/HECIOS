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
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "data_type_processor.h"
#include "file_distribution.h"
#include "filename.h"
#include "pvfs_proto_m.h"
using namespace std;

DataFlow::DataFlow(const spfsDataFlowStart& flowStart,
                   size_t numBuffers,
                   FSSize bufferSize,
                   cSimpleModule* parentModule)
    : originatingMessage_(&flowStart),
      numBuffers_(numBuffers),
      bufferSize_(bufferSize),
      parentModule_(parentModule),
      mode_(static_cast<Mode>(flowStart.getFlowMode())),
      uniqueId_(simulation.getUniqueNumber()),
      flowSize_(0),
      networkTransferTotal_(0),
      storageTransferTotal_(0),
      transferFromNetworkDelay_("SPFS Flow from Network Delay"),
      transferFromStorageDelay_("SPFS Flow from Storage Delay"),
      transferToNetworkDelay_("SPFS Flow to Network Delay"),
      transferToStorageDelay_("SPFS Flow to Storage Delay")
{
    // Create the data type layout
    if (CLIENT_READ == mode_ || CLIENT_WRITE == mode_)
    {
        FSSize aggregateSize;
        flowSize_ = DataTypeProcessor::createFileLayoutForClient(
            flowStart.getOffset(),
            *(flowStart.getDataType()),
            flowStart.getCount(),
            *(flowStart.getView()),
            *(flowStart.getDist()),
            aggregateSize);
        layout_.addRegion(0, flowSize_);
    }
    else if (SERVER_READ == mode_ || SERVER_WRITE == mode_)
    {
        flowSize_ = DataTypeProcessor::createFileLayoutForServer(
            flowStart.getOffset(),
            flowStart.getDataSize(),
            *(flowStart.getView()),
            *(flowStart.getDist()),
            layout_);
    }
    assert(0 < flowSize_);
}

DataFlow::~DataFlow()
{
}

spfsDataFlowStart* DataFlow::getOriginatingMessage() const
{
    return const_cast<spfsDataFlowStart*>(originatingMessage_);
}

void DataFlow::initialize()
{
    // Spawn the first operations
    for (size_t i = 0; i < numBuffers_; i++)
    {
        startTransfer();
    }
}

bool DataFlow::isComplete() const
{
    return (flowSize_ == networkTransferTotal_) &&
        (flowSize_ == storageTransferTotal_);
}

bool DataFlow::isNetworkComplete() const
{
    return (flowSize_ == networkTransferTotal_);
}

bool DataFlow::isStorageComplete() const
{
    return (flowSize_ == storageTransferTotal_);
}

void DataFlow::handleServerMessage(cMessage* msg)
{
    assert(0 != msg);
    processDataFlowMessage(msg);
}

void DataFlow::addNetworkProgress(FSSize dataTransferred)
{
    networkTransferTotal_ += dataTransferred;
}

void DataFlow::addStorageProgress(FSSize dataTransferred)
{
    storageTransferTotal_ += dataTransferred;
}

void DataFlow::collectTransferFromNetworkDelay(cMessage* response)
{
    simtime_t delay = getRoundTripDelay(response);
    transferFromNetworkDelay_.record(delay);
}

void DataFlow::collectTransferFromStorageDelay(cMessage* response)
{
    simtime_t delay = getRoundTripDelay(response);
    transferFromStorageDelay_.record(delay);
}

void DataFlow::collectTransferToNetworkDelay(cMessage* response)
{
    simtime_t delay = getRoundTripDelay(response);
    transferToNetworkDelay_.record(delay);
}

void DataFlow::collectTransferToStorageDelay(cMessage* response)
{
    simtime_t delay = getRoundTripDelay(response);
    transferToStorageDelay_.record(delay);
}

simtime_t DataFlow::getRoundTripDelay(cMessage* response) const
{
    // Get the originating request
    cMessage* request = static_cast<cMessage*>(response->contextPointer());

    // Determine the request response roundtrip time
    simtime_t reqSendTime = request->creationTime();
    simtime_t respArriveTime = parentModule_->simTime();
    return (respArriveTime - reqSendTime);
}


/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
