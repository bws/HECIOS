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
      connectionId_(0),
      pullSubregionOffset_(0),
      pushSubregionOffset_(0)
{
    spfsRequest* request =
        static_cast<spfsRequest*>(flowStart.contextPointer());
    connectionId_ = request->getBmiConnectionId();
}

BMIListIODataFlow::~BMIListIODataFlow()
{
}

void BMIListIODataFlow::processDataFlowMessage(cMessage* msg)
{
    // Variables to hold the next processing function and buffer size
    void (BMIListIODataFlow::*nextProcessFunction)(FSSize) = 0;
    FSSize nextBufferSize = 0;
    
    // Update the progress based on the results of the received message
    // and determine the next processing step
    //
    // Note: the function pointer is accessing a member function, thus
    // the slightly clunky syntax, the function is invoked further below
    //
    // Note that we cannot use message kinds here b/c the BMI messages
    // have been over the network and have had their kind field modified
    //
    if (0 != dynamic_cast<spfsBMIPullDataResponse*>(msg))
    {
        spfsBMIPullDataResponse* pullResp =
            static_cast<spfsBMIPullDataResponse*>(msg);
        FSSize dataSize = pullResp->getDataSize();
        addClientProgress(dataSize);

        // Indicate the next processing step
        nextProcessFunction = &BMIListIODataFlow::pushDataToStorage;
        nextBufferSize = dataSize;
    }
    else if (0 != dynamic_cast<spfsBMIPushDataResponse*>(msg))
    {
        spfsBMIPushDataResponse* pushResp =
            static_cast<spfsBMIPushDataResponse*>(msg);
        addClientProgress(pushResp->getReceivedSize());
        
        // Indicate the next processing step
        nextProcessFunction = &BMIListIODataFlow::pullDataFromStorage;
        nextBufferSize = getBufferSize();
    }
    else if (0 != dynamic_cast<spfsOSFileReadResponse*>(msg))
    {
        spfsOSFileReadResponse* readResp =
            static_cast<spfsOSFileReadResponse*>(msg);
        FSSize bytesRead = readResp->getBytesRead();
        addStorageProgress(bytesRead);
        
        // Indicate the next processing step
        nextProcessFunction = &BMIListIODataFlow::pushDataToClient;
        nextBufferSize = bytesRead;
    }
    else if (0 != dynamic_cast<spfsOSFileWriteResponse*>(msg))
    {
        spfsOSFileWriteResponse* writeResp =
            static_cast<spfsOSFileWriteResponse*>(msg);
        addStorageProgress(writeResp->getBytesWritten());

        // Indicate the next processing step
        nextProcessFunction = &BMIListIODataFlow::pullDataFromClient;
        nextBufferSize = getBufferSize();
    }
    
    assert(0 != nextProcessFunction);
    assert(0 != nextBufferSize);
    // If this is the last flow message response, send the final response
    // Else process the response
    if (isComplete())
    {
        // Send the final response
        spfsDataFlowFinish* flowFinish =
            new spfsDataFlowFinish(0, SPFS_DATA_FLOW_FINISH);
        flowFinish->setContextPointer(getOriginatingMessage());
        flowFinish->setFlowId(getUniqueId());
        module_->scheduleAt(module_->simTime(), flowFinish);
    }
    else
    {
        // Call the next processing function with the correct buffer size
        //
        // Note: again, member function pointer syntax is a little odd
        //
        (this->*nextProcessFunction)(nextBufferSize);
    }

    // Cleanup the messages
    delete (cMessage*)msg->contextPointer();
    delete msg;
}

void BMIListIODataFlow::pullDataFromClient(FSSize pullSize)
{
    // If more data is available, pull it
    if (pullSubregionOffset_ < getSize())
    {
        // Extract the region size to request
        FSSize bufferSize = min(getSize() - pullSubregionOffset_,
                                pullSize);
        pullSubregionOffset_ += bufferSize;
        
        spfsBMIPullDataRequest* pullRequest = new spfsBMIPullDataRequest();
        spfsDataFlowStart* startMsg = getOriginatingMessage();
        pullRequest->setContextPointer(startMsg);
        pullRequest->setFlowId(getUniqueId());
        pullRequest->setFlowSize(getSize());
        pullRequest->setHandle(startMsg->getHandle());
        pullRequest->setConnectionId(connectionId_);
        pullRequest->setRequestSize(bufferSize);
        pullRequest->setByteLength(sizeof(bufferSize));
        
        // Send the pull request
        module_->send(pullRequest, "netOut");    
    }
}

void BMIListIODataFlow::pushDataToClient(FSSize pushSize)
{
    spfsBMIPushDataRequest* pushRequest = new spfsBMIPushDataRequest();
    spfsDataFlowStart* startMsg = getOriginatingMessage();
    pushRequest->setContextPointer(startMsg);
    pushRequest->setFlowId(getUniqueId());
    pushRequest->setFlowSize(getSize());
    pushRequest->setHandle(startMsg->getHandle());
    pushRequest->setConnectionId(connectionId_);
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

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
