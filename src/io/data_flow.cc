//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "data_flow.h"
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "data_type_processor.h"
#include "file_builder.h"
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
      type_(static_cast<Type>(flowStart.getFlowType())),
      uniqueId_(simulation.getUniqueNumber()),
      flowSize_(0),
      networkTransferTotal_(0),
      storageTransferTotal_(0)
{
    // Create the data type layout
    if (CACHE_FLOW_TYPE == type_)
    {
        initCacheFlow();
    }
    else if (CLIENT_FLOW_TYPE == type_)
    {
        initClientFlow();
    }
    else if (SERVER_FLOW_TYPE == type_)
    {
        initServerFlow();
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
    //      transferFromNetworkDelay_("SPFS Flow from Network Delay"),
    //      transferFromStorageDelay_("SPFS Flow from Storage Delay"),
    //      transferToNetworkDelay_("SPFS Flow to Network Delay"),
    //      transferToStorageDelay_("SPFS Flow to Storage Delay")
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
//    simtime_t delay = getRoundTripDelay(response);
//    transferFromNetworkDelay_.record(delay);
}

void DataFlow::collectTransferFromStorageDelay(cMessage* response)
{
//    simtime_t delay = getRoundTripDelay(response);
//    transferFromStorageDelay_.record(delay);
}

void DataFlow::collectTransferToNetworkDelay(cMessage* response)
{
//    simtime_t delay = getRoundTripDelay(response);
//    transferToNetworkDelay_.record(delay);
}

void DataFlow::collectTransferToStorageDelay(cMessage* response)
{
//    simtime_t delay = getRoundTripDelay(response);
//    transferToStorageDelay_.record(delay);
}

void DataFlow::initCacheFlow()
{
    const spfsCacheDataFlowStart* cacheFlow =
        dynamic_cast<const spfsCacheDataFlowStart*>(originatingMessage_);
    assert(0 != cacheFlow);
    flowSize_ = cacheFlow->getPageSize() * cacheFlow->getNumPages();
    layout_.addRegion(0, flowSize_);
}

void DataFlow::initClientFlow()
{
    const spfsClientDataFlowStart* clientFlow =
        dynamic_cast<const spfsClientDataFlowStart*>(originatingMessage_);
    assert(0 != clientFlow);
    FSSize aggregateSize;
    if (READ_MODE == mode_)
    {
        flowSize_ = DataTypeProcessor::createClientFileLayoutForRead(
            clientFlow->getOffset(),
            *(clientFlow->getDataType()),
            clientFlow->getCount(),
            *(clientFlow->getView()),
            *(clientFlow->getDist()),
            clientFlow->getBstreamSize(),
            aggregateSize);
    }
    else
    {
        flowSize_ = DataTypeProcessor::createClientFileLayoutForWrite(
            clientFlow->getOffset(),
            *(clientFlow->getDataType()),
            clientFlow->getCount(),
            *(clientFlow->getView()),
            *(clientFlow->getDist()),
            aggregateSize);
    }
    layout_.addRegion(0, flowSize_);
}

void DataFlow::initServerFlow()
{
    const spfsServerDataFlowStart* serverFlow =
        dynamic_cast<const spfsServerDataFlowStart*>(originatingMessage_);
    assert(0 != serverFlow);
    if (READ_MODE == mode_)
    {
        flowSize_ = DataTypeProcessor::createServerFileLayoutForRead(
            serverFlow->getOffset(),
            serverFlow->getDataSize(),
            *(serverFlow->getView()),
            *(serverFlow->getDist()),
            serverFlow->getBstreamSize(),
            layout_);
    }
    else
    {
        flowSize_ = DataTypeProcessor::createServerFileLayoutForWrite(
            serverFlow->getOffset(),
            serverFlow->getDataSize(),
            *(serverFlow->getView()),
            *(serverFlow->getDist()),
            layout_);
    }
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
