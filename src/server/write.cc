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

#include "write.h"
#include <cassert>
#include <omnetpp.h>
#include "data_flow.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

Write::Write(FSServer* module, spfsWriteRequest* writeReq)
    : module_(module),
      writeReq_(writeReq)
{
}

void Write::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = writeReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        START_DATA_FLOW = FSM_Transient(1),
        SEND_RESPONSE = FSM_Steady(2),
        SEND_COMPLETION_RESPONSE = FSM_Steady(3),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsWriteRequest*>(msg));
            FSM_Goto(currentState, START_DATA_FLOW);
            break;
        }
        case FSM_Enter(START_DATA_FLOW):
        {
            assert(0 != dynamic_cast<spfsWriteRequest*>(msg));
            startDataFlow();
            break;
        }
        case FSM_Exit(START_DATA_FLOW):
        {
            FSM_Goto(currentState, SEND_RESPONSE);
            break;
        }
        case FSM_Enter(SEND_RESPONSE):
        {
            sendResponse();
            break;
        }
        case FSM_Exit(SEND_RESPONSE):
        {
            FSM_Goto(currentState, SEND_COMPLETION_RESPONSE);
            break;
        }
        case FSM_Enter(SEND_COMPLETION_RESPONSE):
        {
            sendCompletionResponse();
            break;
        }
    }

    // Store current state
    writeReq_->setState(currentState);
}

void Write::startDataFlow()
{
    // Construct the data flow start message
    spfsDataFlowStart* dataFlowStart =
        new spfsDataFlowStart(0, SPFS_DATA_FLOW_START);
    dataFlowStart->setContextPointer(writeReq_);

    // Set the flow configuration
    dataFlowStart->setFlowType(SPFS_BMI_TO_LIST_IO_FLOW);
    dataFlowStart->setFlowMode(DataFlow::SERVER_WRITE);

    // Set the BMI connection parameters
    dataFlowStart->setBmiConnectionId(writeReq_->getBmiConnectionId());
    dataFlowStart->setInboundBmiTag(writeReq_->getServerFlowBmiTag());
    dataFlowStart->setOutboundBmiTag(writeReq_->getClientFlowBmiTag());

    // Data transfer configuration
    dataFlowStart->setHandle(writeReq_->getHandle());
    dataFlowStart->setOffset(writeReq_->getOffset());
    dataFlowStart->setDataSize(writeReq_->getDataSize());
    dataFlowStart->setView(writeReq_->getView());
    dataFlowStart->setDist(writeReq_->getDist());

    module_->send(dataFlowStart);
}

void Write::sendResponse()
{
    spfsWriteResponse* resp = new spfsWriteResponse(
        0, SPFS_WRITE_RESPONSE);
    resp->setContextPointer(writeReq_);

    // Set message length for buffer_req
    resp->setByteLength(8);
    module_->send(resp);
}

void Write::sendCompletionResponse()
{
    spfsWriteCompletionResponse* resp = new spfsWriteCompletionResponse(
        0, SPFS_WRITE_COMPLETION_RESPONSE);
    resp->setContextPointer(writeReq_);

    // Set message length for total_written
    resp->setByteLength(8);
    module_->send(resp);
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
