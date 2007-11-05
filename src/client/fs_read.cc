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
#include "fs_read.h"
#include <iostream>
#include <numeric>
#define FSM_DEBUG  // Enable FSM Debug output
#include <omnetpp.h>
#include "data_type_layout.h"
#include "data_type_processor.h"
#include "file_distribution.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSRead::FSRead(FSClient* client, spfsMPIFileReadAtRequest* readReq)
    : client_(client),
      readReq_(readReq)
{
    assert(0 != client_);
    assert(0 != readReq_);
}

// Processing that occurs upon receipt of an MPI-IO read request
void FSRead::handleMessage(cMessage* msg)
{
    /** Restore the existing state for this Read Request */
    cFSM currentState = readReq_->getState();

    /** File system read state machine states
     *
     *  Note that the READ state is transient to facilitate correct
     *  response counting.  Responses are counted upon exit of the
     *  COUNT_* state rather than upon entry
     */
    enum {
        INIT = 0,
        READ = FSM_Transient(1),
        COUNT = FSM_Steady(2),
        COUNT_SERVER_RESPONSE = FSM_Transient(2),
        COUNT_FLOW_FINISH = FSM_Transient(3),
        FINISH = FSM_Steady(3),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileReadAtRequest*>(msg));
            FSM_Goto(currentState, READ);
            break;
        }
        case FSM_Enter(READ):
        {
            assert(0 != dynamic_cast<spfsMPIFileReadAtRequest*>(msg));
            enterRead();
            break;
        }
        case FSM_Exit(READ):
        {
            assert(0 != dynamic_cast<spfsMPIFileReadAtRequest*>(msg));
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
            assert(0 != dynamic_cast<spfsDataFlowFinish*>(msg));
            countFlowFinish();
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
            finish();
            break;
        }
    }

    // Store current state
    readReq_->setState(currentState);
}

void FSRead::enterRead()
{
    assert(0 != readReq_->getFileDes());
    FSOpenFile* filedes = readReq_->getFileDes();
    
    // Construct the template read request
    spfsReadRequest read("ReadStuff", SPFS_READ_REQUEST);
    read.setContextPointer(readReq_);
    read.setServerCnt(filedes->metaData->dataHandles.size());
    read.setOffset(readReq_->getOffset());
    read.setCount(readReq_->getCount());
    read.setDataType(readReq_->getDataType());

    // Send request to each server
    size_t numRequests = 0;
    for (int i = 0; i < read.getServerCnt(); i++)
    {
        // Process the data type to determine the read size
        DataTypeLayout layout;
        filedes->metaData->dist->setObjectIdx(i);
        DataTypeProcessor::createFileLayoutForClient(read.getOffset(),
                                                     read.getDataType(),
                                                     read.getCount(),
                                                     *filedes->metaData->dist,
                                                     10000000,
                                                     layout);

        // Sum all the extents to determine total read size
        size_t reqBytes = layout.getLength();

        if (0 != reqBytes)
        {
            // Set the message size in bytes
            spfsReadRequest* req = static_cast<spfsReadRequest*>(read.dup());
            req->setHandle(filedes->metaData->dataHandles[i]);
            req->setDist(filedes->metaData->dist->clone());
            req->setByteLength(4 + 8 + 8 + 8);
            client_->send(req, client_->getNetOutGate());

            // Increment the number of outstanding requests
            numRequests++;
        }
    }

    // Set the number of outstanding responses and flows
    readReq_->setRemainingResponses(numRequests);
    readReq_->setRemainingFlows(numRequests);
}

void FSRead::countResponse()
{
    int numRemainingResponses = readReq_->getRemainingResponses();
    readReq_->setRemainingResponses(--numRemainingResponses);
}

void FSRead::countFlowFinish()
{
    int numRemainingFlows = readReq_->getRemainingFlows();
    readReq_->setRemainingFlows(--numRemainingFlows);
}

bool FSRead::isReadComplete()
{
    return (0 == readReq_->getRemainingResponses())
        && (0 == readReq_->getRemainingFlows());
}

void FSRead::finish()
{
    spfsMPIFileReadAtResponse* mpiResp =
        new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
    mpiResp->setContextPointer(readReq_);
    mpiResp->setIsSuccessful(true);
    mpiResp->setBytesRead(0);  // FIXME
    client_->send(mpiResp, client_->getAppOutGate());
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
