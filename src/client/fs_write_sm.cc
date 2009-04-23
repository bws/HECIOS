//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
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
#include "fs_write_sm.h"
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "data_flow.h"
#include "data_type_processor.h"
#include "file_distribution.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSWriteSM::FSWriteSM(spfsMPIFileWriteAtRequest* writeRequest,
                     FSClient* client)
    : writeRequest_(writeRequest),
      client_(client),
      bytesWritten_(0)
{
    assert(0 != writeRequest);
    assert(0 != client);
}

bool FSWriteSM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system write state machine states */
    enum {
        INIT = 0,
        WRITE = FSM_Transient(1),
        COUNT = FSM_Steady(2),
        COUNT_RESPONSE = FSM_Transient(3),
        COUNT_FLOW_FINISH = FSM_Transient(4),
        COUNT_WRITE_COMPLETION = FSM_Transient(5),
        FINISH = FSM_Steady(6),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, WRITE);
            break;
        }
        case FSM_Enter(WRITE):
        {
            beginWrite();
            break;
        }
        case FSM_Exit(WRITE):
        {
            FSM_Goto(currentState, COUNT);
            break;
        }
        case FSM_Exit(COUNT):
        {
            if (0 != dynamic_cast<spfsWriteResponse*>(msg))
            {
                FSM_Goto(currentState, COUNT_RESPONSE);
            }
            else if (0 != dynamic_cast<spfsDataFlowFinish*>(msg))
            {
                FSM_Goto(currentState, COUNT_FLOW_FINISH);
            }
            else
            {
                assert(0 != dynamic_cast<spfsWriteCompletionResponse*>(msg));
                FSM_Goto(currentState, COUNT_WRITE_COMPLETION);
            }
            break;
        }
        case FSM_Enter(COUNT_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsWriteResponse*>(msg));
            startFlow(dynamic_cast<spfsWriteResponse*>(msg));
            break;
        }
        case FSM_Exit(COUNT_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsWriteResponse*>(msg));
            countResponse();
            FSM_Goto(currentState, COUNT);
            break;
        }
        case FSM_Exit(COUNT_FLOW_FINISH):
        {
            countFlowFinish(dynamic_cast<spfsDataFlowFinish*>(msg));
            if (isWriteComplete())
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT);
            }
            break;
        }
        case FSM_Exit(COUNT_WRITE_COMPLETION):
        {
            countCompletion(dynamic_cast<spfsWriteCompletionResponse*>(msg));
            if (isWriteComplete())
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

void FSWriteSM::beginWrite()
{
    assert(0 != writeRequest_->getFileDes());
    FileDescriptor* fd = writeRequest_->getFileDes();
    const FSMetaData* metaData = fd->getMetaData();

    // Send request to each server
    int numRequests = 0;
    int numServers = metaData->dataHandles.size();
    for (int i = 0; i < numServers; i++)
    {
        // Process the data type to determine the write size
        metaData->dist->setObjectIdx(i);
        FSSize aggregateSize = 0;
        FSSize reqBytes = DataTypeProcessor::createClientFileLayoutForWrite(
            writeRequest_->getOffset(),
            *writeRequest_->getDataType(),
            writeRequest_->getCount(),
            fd->getFileView(),
            *metaData->dist,
            aggregateSize);

        // Send write request if server hosts data
        if (0 != reqBytes)
        {
            spfsWriteRequest* req = FSClient::createWriteRequest(
                metaData->handle,
                metaData->dataHandles[i],
                fd->getFileView(),
                writeRequest_->getOffset(),
                aggregateSize,
                *(metaData->dist));
            req->setContextPointer(writeRequest_);

            // Disable auto cleanup, this request receives several responses
            req->setAutoCleanup(false);
            client_->send(req, client_->getNetOutGate());

            // Add to the number of requests sent
            numRequests++;
        }
    }

    // Set the number of responses
    writeRequest_->setRemainingResponses(numRequests);
    writeRequest_->setRemainingFlows(numRequests);
    writeRequest_->setRemainingCompletions(numRequests);
}

void FSWriteSM::startFlow(spfsWriteResponse* writeResponse)
{
    // Extract the server request
    spfsWriteRequest* serverRequest =
        static_cast<spfsWriteRequest*>(writeResponse->contextPointer());

    // Create the flow start message
    spfsClientDataFlowStart* flowStart =
        new spfsClientDataFlowStart(0, SPFS_DATA_FLOW_START);
    flowStart->setContextPointer(writeRequest_);

    // Set the handle as the connection id (TODO: This is hacky)
    flowStart->setBmiConnectionId(serverRequest->getHandle());
    flowStart->setInboundBmiTag(serverRequest->getClientFlowBmiTag());
    flowStart->setOutboundBmiTag(serverRequest->getServerFlowBmiTag());

    // Flow configuration
    flowStart->setFlowType(DataFlow::CLIENT_FLOW_TYPE);
    flowStart->setFlowMode(DataFlow::WRITE_MODE);

    // Data transfer configuration
    flowStart->setHandle(serverRequest->getHandle());
    flowStart->setOffset(writeRequest_->getOffset());
    flowStart->setDataType(writeRequest_->getDataType());
    flowStart->setCount(writeRequest_->getCount());
    flowStart->setView(serverRequest->getView());
    flowStart->setDist(serverRequest->getDist());

    // Send the start message
    client_->send(flowStart, client_->getNetOutGate());
}

void FSWriteSM::countResponse()
{
    int numRemainingResponses = writeRequest_->getRemainingResponses();
    writeRequest_->setRemainingResponses(--numRemainingResponses);
}

void FSWriteSM::countFlowFinish(spfsDataFlowFinish* finishMsg)
{
    assert(0 != finishMsg);
    bytesWritten_ += finishMsg->getFlowSize();
    int numRemainingFlows = writeRequest_->getRemainingFlows();
    writeRequest_->setRemainingFlows(--numRemainingFlows);
}

void FSWriteSM::countCompletion(spfsWriteCompletionResponse* completionResponse)
{
    int numRemainingCompletions = writeRequest_->getRemainingCompletions();
    writeRequest_->setRemainingCompletions(--numRemainingCompletions);

    // Cleanup the PFS request
    spfsWriteRequest* pfsReq =
        (spfsWriteRequest*)completionResponse->contextPointer();
    pfsReq->setAutoCleanup(true);
    delete pfsReq->getDist();
    delete pfsReq->getView();
}

bool FSWriteSM::isWriteComplete()
{
    bool isComplete = (0 == writeRequest_->getRemainingResponses()) &&
                      (0 == writeRequest_->getRemainingFlows()) &&
                      (0 == writeRequest_->getRemainingCompletions());
    return isComplete;
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
