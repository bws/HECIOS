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
#include "fs_write.h"
#include <cassert>
#include <iostream>
#include <numeric>
#include <omnetpp.h>
#include "data_type_layout.h"
#include "data_type_processor.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
#include "file_distribution.h"
using namespace std;

FSWrite::FSWrite(FSClient* client, spfsMPIFileWriteAtRequest* writeReq)
    : client_(client),
      writeReq_(writeReq),
      bytesWritten_(0)
{
    assert(0 != client_);
    assert(0 != writeReq_);
}

// Processing that occurs upon receipt of an MPI-IO Write request
void FSWrite::handleMessage(cMessage* msg)
{
    /** Restore the existing state for this request */
    cFSM currentState = writeReq_->getState();

    /** File system write state machine states
     *
     *  Note that the WRITE state is transient to facilitate correct
     *  response counting.  Responses are counted upon exit of the
     *  COUNT_* states rather than upon entry
     */
    enum {
        INIT = 0,
        WRITE = FSM_Transient(1),
        COUNT = FSM_Steady(2),
        COUNT_RESPONSE = FSM_Transient(3),
        COUNT_FLOW_FINISH = FSM_Transient(4),
        COUNT_WRITE_COMPLETION = FSM_Transient(5),
        FINISH = FSM_Steady(6),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileWriteAtRequest*>(msg));
            FSM_Goto(currentState, WRITE);
            break;
        }
        case FSM_Enter(WRITE):
        {
            assert(0 != dynamic_cast<spfsMPIFileWriteAtRequest*>(msg));
            beginWrite();
            break;
        }
        case FSM_Exit(WRITE):
        {
            assert(0 != dynamic_cast<spfsMPIFileWriteAtRequest*>(msg));
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
        case FSM_Exit(COUNT_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsWriteResponse*>(msg));
            countResponse();
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
            
            // Delete originating request's distribution
            //spfsWriteRequest* req = (spfsWriteRequest*)msg->contextPointer();
            //delete req->getDist();
            //delete req;
            break;
        }
        case FSM_Enter(FINISH):
        {
            finish();

            // Cleanup the PFS request
            spfsWriteRequest* pfsReq = (spfsWriteRequest*)msg->contextPointer();
            delete pfsReq->getDist();
            delete pfsReq;
            break;
        }
    }

    // Store current state
    writeReq_->setState(currentState);
}

void FSWrite::beginWrite()
{
    assert(0 != writeReq_->getFileDes());
    FSOpenFile* filedes = (FSOpenFile*)writeReq_->getFileDes();
    
    // Construct the server write request
    spfsWriteRequest write(0, SPFS_WRITE_REQUEST);
    write.setContextPointer(writeReq_);
    write.setServerCnt(filedes->metaData->dataHandles.size());
    write.setOffset(filedes->filePtr);
    write.setCount(writeReq_->getCount());
    write.setDataType(writeReq_->getDataType());

    // Send request to each server
    int numRequests = 0;
    for (int i = 0; i < write.getServerCnt(); i++)
    {
        // Process the data type to determine the write size
        DataTypeLayout layout;
        filedes->metaData->dist->setObjectIdx(i);
        DataTypeProcessor::createFileLayoutForClient(write.getOffset(),
                                                     write.getDataType(),
                                                     write.getCount(),
                                                     *filedes->metaData->dist,
                                                     10000000,
                                                     layout);

        // Sum all the extents to determine total write size
        size_t reqBytes = layout.getLength();

        // Send write request if server hosts data
        if (0 != reqBytes)
        {
            // Set the message size in bytes
            spfsWriteRequest* req = static_cast<spfsWriteRequest*>(
                write.dup());
            req->setHandle(filedes->metaData->dataHandles[i]);
            req->setDist(filedes->metaData->dist->clone());
            req->setByteLength(4 + 8 + 8 + 8);
            client_->send(req, client_->getNetOutGate());

            // Increment the number of outstanding requests
            numRequests++;
        }
    }

    // Set the number of responses
    writeReq_->setRemainingResponses(numRequests);
    writeReq_->setRemainingFlows(numRequests);
    writeReq_->setRemainingCompletions(numRequests);
}

void FSWrite::countResponse()
{
    int numRemainingResponses = writeReq_->getRemainingResponses();
    writeReq_->setRemainingResponses(--numRemainingResponses);
}

void FSWrite::countFlowFinish(spfsDataFlowFinish* finishMsg)
{
    assert(0 != finishMsg);
    bytesWritten_ += finishMsg->getFlowSize();
    int numRemainingFlows = writeReq_->getRemainingFlows();
    writeReq_->setRemainingFlows(--numRemainingFlows);
}

void FSWrite::countCompletion(spfsWriteCompletionResponse* completionResponse)
{
    int numRemainingCompletions = writeReq_->getRemainingCompletions();
    writeReq_->setRemainingCompletions(--numRemainingCompletions);
}

bool FSWrite::isWriteComplete()
{
    return (0 == writeReq_->getRemainingResponses())
        && (0 == writeReq_->getRemainingFlows())
        && (0 == writeReq_->getRemainingCompletions());
}

void FSWrite::finish()
{
    spfsMPIFileWriteAtResponse* mpiResp =
        new spfsMPIFileWriteAtResponse(0, SPFS_MPI_FILE_WRITE_AT_RESPONSE);
    mpiResp->setContextPointer(writeReq_);
    mpiResp->setIsSuccessful(true);
    client_->send(mpiResp, client_->getAppOutGate());
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
