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
#include "bmi_list_io_data_flow.h"
#include <cassert>
#include <vector>
#include "bmi_proto_m.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

BMIListIODataFlow::BMIListIODataFlow(const spfsDataFlowStart& flowStart,
                                     std::size_t numBuffers,
                                     FSSize bufferSize,
                                     cSimpleModule* module)
    : DataFlow(flowStart, numBuffers, bufferSize),
      filename_(flowStart.getHandle()),
      module_(module),
      bmiConnectionId_(flowStart.getBmiConnectionId()),
      bmiTag_(flowStart.getBmiTag()),
      pullSubregionOffset_(0),
      pushSubregionOffset_(0)
{
}

BMIListIODataFlow::~BMIListIODataFlow()
{
    cout << "Flow Completed Net: " << getNetworkProgress()
         << " Store: " << getStorageProgress() << endl;
}

void BMIListIODataFlow::startTransfer()
{
    if (SERVER_READ == getMode())
    {
        // Request the data from storage
        pullDataFromStorage(getBufferSize());
    }
    else if (SERVER_WRITE == getMode())
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

        // Cleanup the originating request
        delete static_cast<cMessage*>(msg->contextPointer());
    }
    else if (0 != dynamic_cast<spfsOSFileWriteResponse*>(msg))
    {
        spfsOSFileWriteResponse* writeResp =
            static_cast<spfsOSFileWriteResponse*>(msg);
        addStorageProgress(writeResp->getBytesWritten());

        // Send the data successfully committed acknowledgement
        sendPushAck(writeResp->getBytesWritten());

        // Cleanup the originating request
        delete static_cast<cMessage*>(msg->contextPointer());
    }
    
    cerr << __FILE__ << ":" << __LINE__ << " "
         << "Net: " << getNetworkProgress() << " "
         << "Store: " << getStorageProgress() << endl;

    // If this is the last flow message response, send the final response
    if (isComplete())
    {
        // Send the final response
        spfsDataFlowFinish* flowFinish =
            new spfsDataFlowFinish(0, SPFS_DATA_FLOW_FINISH);
        flowFinish->setContextPointer(getOriginatingMessage());
        flowFinish->setFlowId(getUniqueId());
        module_->scheduleAt(module_->simTime(), flowFinish);
        cerr << "Finishing BMI-ListIO flow\n";
    }

    // Cleanup the received message
    delete msg;
}

void BMIListIODataFlow::pullDataFromNetwork(FSSize pullSize)
{
}

void BMIListIODataFlow::pushDataToNetwork(FSSize pushSize)
{
    spfsBMIPushDataRequest* pushRequest = new spfsBMIPushDataRequest();
    spfsDataFlowStart* startMsg = getOriginatingMessage();
    pushRequest->setContextPointer(startMsg);

    // Set BMI network fields
    pushRequest->setTag(bmiTag_);
    pushRequest->setConnectionId(bmiConnectionId_);

    // Set flow data
    pushRequest->setFlowId(getUniqueId());
    pushRequest->setFlowSize(getSize());
    pushRequest->setHandle(startMsg->getHandle());
    pushRequest->setDataSize(pushSize);
    pushRequest->setByteLength(pushSize);
    
    // Send the push request
    module_->send(pushRequest, "netOut");
}

void BMIListIODataFlow::pullDataFromStorage(FSSize pullSize)
{
    // If more data is available, pull it
    if (pullSubregionOffset_ < getSize())
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
    pushResponse->setTag(bmiTag_);
    pushResponse->setReceivedSize(amountRecvd);
    pushResponse->setByteLength(16);

    // Send the push request
    module_->send(pushResponse, "netOut");
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