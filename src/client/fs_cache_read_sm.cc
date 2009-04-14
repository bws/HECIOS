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
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_cache_read_sm.h"
#include <cassert>
#include <omnetpp.h>
#include "cache_proto_m.h"
#include "data_flow.h"
#include "data_type_layout.h"
#include "data_type_processor.h"
#include "file_distribution.h"
#include "fs_client.h"
#include "pvfs_proto_m.h"
using namespace std;

FSCacheReadSM::FSCacheReadSM(spfsCacheReadRequest* readRequest,
                             FSClient* client)
    : readRequest_(readRequest),
      client_(client),
      bytesRead_(0)
{
    assert(0 != readRequest_);
    assert(0 != client_);
}

bool FSCacheReadSM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system write state machine states */
    enum {
        INIT = 0,
        READ = FSM_Transient(1),
        COUNT = FSM_Steady(2),
        COUNT_SERVER_RESPONSE = FSM_Transient(2),
        COUNT_FLOW_FINISH = FSM_Transient(3),
        FINISH = FSM_Steady(4),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, READ);
            break;
        }
        case FSM_Enter(READ):
        {
            enterRead();
            break;
        }
        case FSM_Exit(READ):
        {
            if (isReadComplete())
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT);
            }
            break;
        }
        case FSM_Exit(COUNT):
        {
            if (SPFS_READ_RESPONSE == msg->kind())
            {
                FSM_Goto(currentState, COUNT_SERVER_RESPONSE);
            }
            else
            {
                assert(SPFS_DATA_FLOW_FINISH == msg->kind());
                FSM_Goto(currentState, COUNT_FLOW_FINISH);
            }
            break;
        }
        case FSM_Enter(COUNT_SERVER_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsReadResponse*>(msg));

            // Need a quick workaround in case there isn't a need for a
            // flow here
            spfsReadRequest* readRequest =
                static_cast<spfsReadRequest*>(msg->contextPointer());
            if (true == readRequest->getHasReadData())
            {
                startFlow(dynamic_cast<spfsReadResponse*>(msg));
            }
            break;
        }
        case FSM_Exit(COUNT_SERVER_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsReadResponse*>(msg));
            countResponse();

            if (isReadComplete())
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT);
            }
            break;
        }
        case FSM_Exit(COUNT_FLOW_FINISH):
        {
            countFlowFinish(dynamic_cast<spfsDataFlowFinish*>(msg));
            if (isReadComplete())
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT);
            }
            break;
        }
        case FSM_Enter(FINISH):
        {
            isComplete = true;
            break;
        }
    }

    return isComplete;
}

void FSCacheReadSM::enterRead()
{
    /*
    FileDescriptor* fd = readRequest_->getDescriptor();
    assert(0 != fd);
    const FSMetaData* metaData = fd->getMetaData();

    // Construct the template read request
    spfsReadRequest read("ReadStuff", SPFS_READ_REQUEST);

    // Note: The read request is NOT deleted here as one would expect.
    // Because the server side flow completes after the client, the request
    // is deleted on the server
    read.setContextPointer(readRequest_);
    read.setOffset(readRequest_->getOffset());
    read.setView(new FileView(fd->getFileView()));

    // Send request to each server
    int numRequests = 0;
    int numFlows = 0;
    int numServers = metaData->dataHandles.size();
    for (int i = 0; i < numServers; i++)
    {
        // Process the data type to determine the read size
        metaData->dist->setObjectIdx(i);
        FSSize aggregateSize = 0;
        FSSize reqBytes = DataTypeProcessor::createFileLayoutForClient(
            readRequest_->getOffset(),
            *readRequest_->getDataType(),
            readRequest_->getCount(),
            *read.getView(),
            *metaData->dist,
            aggregateSize);

        if (!fileHasReadData(readRequest_->getOffset() + reqBytes))
        {
            // Create a request that the server will not flow data for
            // because the read data is not present in the file
            spfsReadRequest* req = static_cast<spfsReadRequest*>(read.dup());
            req->setAutoCleanup(true);
            req->setHasReadData(false);
            req->setHandle(metaData->dataHandles[i]);
            req->setDataSize(aggregateSize);

            // Set the message size in bytes
            req->setByteLength(8 + 8 + 4 + 4 + 4 +
                               //FSClient::CREDENTIALS_SIZE +
                               fd->getFileView().getRepresentationByteLength() +
                               8 + 8);
            client_->send(req, client_->getNetOutGate());

            // Add to the number of requests sent
            numRequests++;
        }
        else if (0 != reqBytes && 0 != aggregateSize)
        {
            // Note: The read request is NOT deleted here as one would expect.
            // Because the server side flow completes after the client, the request
            // is deleted on the server
            spfsReadRequest* req = static_cast<spfsReadRequest*>(read.dup());
            req->setAutoCleanup(false);

            // Set the server specific request data
            req->setHasReadData(true);
            req->setHandle(metaData->dataHandles[i]);
            req->setDist(metaData->dist->clone());
            req->setDataSize(aggregateSize);
            req->setClientFlowBmiTag(simulation.getUniqueNumber());
            req->setServerFlowBmiTag(simulation.getUniqueNumber());

            // Set the message size in bytes
            req->setByteLength(8 + 8 + 4 + 4 + 4 +
                               //FSClient::CREDENTIALS_SIZE +
                               fd->getFileView().getRepresentationByteLength() +
                               8 + 8);
            client_->send(req, client_->getNetOutGate());

            // Add to the number of requests and flows sent
            numRequests++;
            numFlows++;
        }
    }

    // Set the number of outstanding responses and flows
    readRequest_->setRemainingResponses(numRequests);
    readRequest_->setRemainingFlows(numFlows);
*/
}

bool FSCacheReadSM::fileHasReadData(size_t reqBytes)
{
    // TODO: This is not a correct implementation, but it works for now
    // Extract the file descriptor
    FileDescriptor* fd = readRequest_->getDescriptor();
    assert(0 != fd);
    FSSize fileSize = fd->getMetaData()->size;
    return (reqBytes <= fileSize);
}

void FSCacheReadSM::startFlow(spfsReadResponse* readResponse)
{
    // Extract the file descriptor
    FileDescriptor* fd = readRequest_->getDescriptor();
    assert(0 != fd);

    // Extract the server request
    spfsReadRequest* serverRequest =
        static_cast<spfsReadRequest*>(readResponse->contextPointer());

    // Create the flow start message
    spfsDataFlowStart* flowStart =
        new spfsDataFlowStart(0, SPFS_DATA_FLOW_START);
    flowStart->setContextPointer(readRequest_);
    flowStart->setClientContextPointer(serverRequest);

    // Set the handle as the connection id (TODO: This is hacky)
    flowStart->setBmiConnectionId(serverRequest->getHandle());
    flowStart->setInboundBmiTag(serverRequest->getClientFlowBmiTag());
    flowStart->setOutboundBmiTag(serverRequest->getServerFlowBmiTag());

    // Flow configuration
    flowStart->setFlowType(DataFlow::CACHE_FLOW_TYPE);
    flowStart->setFlowMode(DataFlow::READ_MODE);

    // Data transfer configuration
    flowStart->setHandle(serverRequest->getHandle());
    flowStart->setView(serverRequest->getView());
    flowStart->setDist(serverRequest->getDist());
    //TODO flowStart->setOffset(readRequest_->getOffset());
    //TODO flowStart->setDataType(readRequest_->getDataType());
    //TODO flowStart->setCount(readRequest_->getCount());

    // Send the start message
    client_->send(flowStart, client_->getNetOutGate());
}

void FSCacheReadSM::countResponse()
{
    int numRemainingResponses = readRequest_->getRemainingResponses();
    readRequest_->setRemainingResponses(--numRemainingResponses);
}

void FSCacheReadSM::countFlowFinish(spfsDataFlowFinish* finishMsg)
{
    assert(0 != finishMsg);
    bytesRead_ += finishMsg->getFlowSize();
    //TODO int numRemainingFlows = readRequest_->getRemainingFlows();
    //TODO readRequest_->setRemainingFlows(--numRemainingFlows);
}

bool FSCacheReadSM::isReadComplete()
{
    return false;
//TODO    return (0 == readRequest_->getRemainingResponses())
//TODO        && (0 == readRequest_->getRemainingFlows());
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
