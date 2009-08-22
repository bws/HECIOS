//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "bmi_list_io_data_flow.h"
#include <cassert>
#include <vector>
#include "bmi_proto_m.h"
#include "data_flow_registry.h"
#include "file_builder.h"
#include "os_proto_m.h"
#include "pfs_types.h"
#include "pvfs_proto_m.h"
using namespace std;

BMIListIODataFlow::BMIListIODataFlow(const spfsDataFlowStart& flowStart,
                                     std::size_t numBuffers,
                                     FSSize bufferSize,
                                     cSimpleModule* module)
    : DataFlow(flowStart, numBuffers, bufferSize, module),
      filename_(flowStart.getHandle()),
      module_(module),
      bmiConnectionId_(flowStart.getBmiConnectionId()),
      outboundBmiTag_(flowStart.getOutboundBmiTag()),
      pullSubregionOffset_(0),
      pushSubregionOffset_(0),
      pfsMetaData_(0),
      bstreamSizeIdx_(0)
{
    // Really only need this stuff for writes
    const spfsServerDataFlowStart& serverFlow =
        dynamic_cast<const spfsServerDataFlowStart&>(flowStart);
    FSHandle metaHandle = serverFlow.getMetaHandle();
    pfsMetaData_ = FileBuilder::instance().getMetaData(metaHandle);
    bstreamSizeIdx_ = serverFlow.getDist()->getObjectIdx();
    assert(0 != pfsMetaData_);
}

BMIListIODataFlow::~BMIListIODataFlow()
{
    //cerr << "Flow Completed Net: " << getNetworkProgress()
    //     << " Store: " << getStorageProgress() << endl;
}

void BMIListIODataFlow::startTransfer()
{
    if (READ_MODE == getMode())
    {
        // Request the data from storage
        pullDataFromStorage(getBufferSize());
    }
    else if (WRITE_MODE == getMode())
    {
        // No-op, just wait for data
    }
}

void BMIListIODataFlow::processDataFlowMessage(cMessage* msg)
{
    if (0 != dynamic_cast<spfsBMIPushDataRequest*>(msg))
    {
        spfsBMIPushDataRequest* pushRequest =
            static_cast<spfsBMIPushDataRequest*>(msg);
        FSSize dataSize = pushRequest->getDataSize();
        addNetworkProgress(dataSize);

        // Send the data successfully received acknowledgement
        sendPushAck(dataSize);

        // Perform the next processing step
        pushDataToStorage(dataSize);
    }
    else if (0 != dynamic_cast<spfsBMIPushDataResponse*>(msg))
    {
        spfsBMIPushDataResponse* pushResp =
            static_cast<spfsBMIPushDataResponse*>(msg);
        addNetworkProgress(pushResp->getReceivedSize());

        // Perform the next processing step
        pullDataFromStorage(getBufferSize());
    }
    else if (0 != dynamic_cast<spfsOSFileReadResponse*>(msg))
    {
        spfsOSFileReadResponse* readResp =
            static_cast<spfsOSFileReadResponse*>(msg);
        FSSize bytesRead = readResp->getBytesRead();
        addStorageProgress(bytesRead);

        // Perform the next processing step
        pushDataToNetwork(bytesRead);

        // Perform data collection for flow progress
        collectTransferFromStorageDelay(readResp);

        // Cleanup the originating request
        delete static_cast<cMessage*>(msg->getContextPointer());
    }
    else if (0 != dynamic_cast<spfsOSFileWriteResponse*>(msg))
    {
        spfsOSFileWriteResponse* writeResp =
            static_cast<spfsOSFileWriteResponse*>(msg);
        addStorageProgress(writeResp->getBytesWritten());

        // Perform data collection for flow progress
        collectTransferToStorageDelay(writeResp);

        // Cleanup the originating request
        delete static_cast<cMessage*>(msg->getContextPointer());
    }

    // If this is the last flow message response, send the final response
    if (isComplete())
    {
        // Send the final response
        spfsDataFlowFinish* flowFinish =
            new spfsDataFlowFinish(0, SPFS_DATA_FLOW_FINISH);
        flowFinish->setContextPointer(getOriginatingMessage());
        flowFinish->setFlowId(getUniqueId());
        module_->scheduleAt(simulation.getSimTime(), flowFinish);
        //cerr << "Finishing BMI-ListIO flow\n";
    }

    // Cleanup the received message
    delete msg;
}

void BMIListIODataFlow::pullDataFromNetwork(FSSize pullSize)
{
    assert(false);
}

void BMIListIODataFlow::pushDataToNetwork(FSSize pushSize)
{
    spfsBMIPushDataRequest* pushRequest = new spfsBMIPushDataRequest();
    spfsDataFlowStart* startMsg = getOriginatingMessage();
    pushRequest->setContextPointer(startMsg);

    // Set BMI network fields
    pushRequest->setTag(outboundBmiTag_);
    pushRequest->setConnectionId(bmiConnectionId_);

    // Set flow data
    pushRequest->setFlowId(getUniqueId());
    pushRequest->setFlowSize(getSize());
    pushRequest->setHandle(startMsg->getHandle());
    pushRequest->setDataSize(pushSize);
    pushRequest->setByteLength(pushSize);

    // Send the push request
    module_->send(pushRequest, "netOut");
    //cerr << "Pushing data to network" << endl;
}

void BMIListIODataFlow::pullDataFromStorage(FSSize pullSize)
{
    // If more data is available, pull it
    if (FSSize(pullSubregionOffset_) < getSize())
    {
        // Extract the regions to process
        FSSize bufferSize = min(getSize() - pullSubregionOffset_,
                                pullSize);
        vector<FileRegion> regions = layout_.getSubRegions(pullSubregionOffset_,
                                                           bufferSize);
        pullSubregionOffset_ += bufferSize;

        // Construct the list i/o request
        spfsOSFileReadRequest* fileRead =
            new spfsOSFileReadRequest(0, SPFS_OS_FILE_READ_REQUEST);
        fileRead->setContextPointer(getOriginatingMessage());
        fileRead->setFilename(filename_.c_str());

        // Add the regions to the request
        fileRead->setOffsetArraySize(regions.size());
        fileRead->setExtentArraySize(regions.size());
        for (size_t i = 0; i < regions.size(); i++)
        {
            fileRead->setOffset(i, regions[i].offset);
            fileRead->setExtent(i, regions[i].extent);

            static size_t total = 0;
            total += regions[i].extent;
            //cerr << __FILE__ << ":" << __LINE__ << ":"
            //     << "Server File Read: off: " << regions[i].offset
            //     << " ext: " << regions[i].extent
            //     << " total: " << total << endl;
        }

        // Send the request to the storage layer
        module_->send(fileRead, "storageOut");
    }
}

void BMIListIODataFlow::pushDataToStorage(FSSize pushSize)
{
    // Extract the regions to process
    vector<FileRegion> regions = layout_.getSubRegions(pushSubregionOffset_,
                                                       pushSize);
    pushSubregionOffset_ += pushSize;

    // Construct the list I/O request
    spfsOSFileWriteRequest* fileWrite =
        new spfsOSFileWriteRequest(0, SPFS_OS_FILE_WRITE_REQUEST);
    fileWrite->setContextPointer(getOriginatingMessage());
    fileWrite->setFilename(filename_.c_str());

    // Add the regions to the request
    fileWrite->setOffsetArraySize(regions.size());
    fileWrite->setExtentArraySize(regions.size());
    for (size_t i = 0; i < regions.size(); i++)
    {
        fileWrite->setOffset(i, regions[i].offset);
        fileWrite->setExtent(i, regions[i].extent);
        updateBstreamSize(regions[i].offset + regions[i].extent);
        //cerr << "Writing To Disk Off: " << regions[i].offset << " Ext: " << regions[i].extent << endl;
    }

    // Send the request to the storage layer
    module_->send(fileWrite, "storageOut");
}

void BMIListIODataFlow::sendPushAck(FSSize amountRecvd)
{
    // Send the push acknowedgement, allowing the sender to continue
    spfsBMIPushDataResponse* pushResponse = new spfsBMIPushDataResponse();
    spfsDataFlowStart* startMsg = getOriginatingMessage();
    pushResponse->setContextPointer(startMsg);
    pushResponse->setConnectionId(bmiConnectionId_);
    pushResponse->setFlowId(getUniqueId());
    pushResponse->setTag(outboundBmiTag_);
    pushResponse->setReceivedSize(amountRecvd);
    pushResponse->setByteLength(0);

    // Look up the partner flow to send a direct out-of-band message
    DataFlow* partner =
        DataFlowRegistry::instance().getSubscribedDataFlow(outboundBmiTag_);
    assert(0 != partner);
    cModule* partnerModule = partner->parentModule();
    assert(0 != partnerModule);

    // Send the acknowledgement directly to module
    parentModule()->sendDirect(pushResponse, 0.0, 0.0, partnerModule, "directIn");
}

void BMIListIODataFlow::updateBstreamSize(FSSize lastByteOffset)
{
    assert(0 != pfsMetaData_);
    FSSize oldSize = pfsMetaData_->bstreamSizes[bstreamSizeIdx_];
    pfsMetaData_->bstreamSizes[bstreamSizeIdx_] = max(lastByteOffset, oldSize);
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
