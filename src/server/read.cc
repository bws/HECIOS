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

#include "read.h"
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "data_flow.h"
#include "file_distribution.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

Read::Read(FSServer* module, spfsReadRequest* readReq)
    : module_(module),
      readReq_(readReq),
      cleanupRequest_(false)
{
}

Read::~Read()
{
    if (cleanupRequest_)
    {
        // TODO: Fix this leak
        //delete readReq_->getView();
        delete readReq_->getDist();
        delete readReq_;
        readReq_ = 0;
    }
}

void Read::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = readReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        START_DATA_FLOW = FSM_Transient(1),
        SEND_FINAL_RESPONSE = FSM_Steady(2),
        FINISH = FSM_Steady(3),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            FSM_Goto(currentState, START_DATA_FLOW);
            break;
        }
        case FSM_Enter(START_DATA_FLOW):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            startDataFlow();
            break;
        }
        case FSM_Exit(START_DATA_FLOW):
        {
            FSM_Goto(currentState, SEND_FINAL_RESPONSE);
            break;
        }
        case FSM_Enter(SEND_FINAL_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            sendFinalResponse();
            break;
        }
        case FSM_Exit(SEND_FINAL_RESPONSE):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsDataFlowFinish*>(msg));
            finish();
            break;
        }
    }

    // Store current state
    readReq_->setState(currentState);
}

void Read::startDataFlow()
{
    // Construct the data flow start message
    spfsDataFlowStart* dataFlowStart =
        new spfsDataFlowStart(0, SPFS_DATA_FLOW_START);
    dataFlowStart->setContextPointer(readReq_);

    // Set the flow configuration
    dataFlowStart->setFlowType(0);
    dataFlowStart->setFlowMode(DataFlow::SERVER_READ);

    // Set the BMI connection parameters
    dataFlowStart->setBmiConnectionId(readReq_->getBmiConnectionId());
    dataFlowStart->setInboundBmiTag(readReq_->getServerFlowBmiTag());
    dataFlowStart->setOutboundBmiTag(readReq_->getClientFlowBmiTag());

    // Data transfer configuration
    dataFlowStart->setHandle(readReq_->getHandle());
    dataFlowStart->setOffset(readReq_->getOffset());
    dataFlowStart->setDataSize(readReq_->getDataSize());
    dataFlowStart->setView(readReq_->getView());
    dataFlowStart->setDist(readReq_->getDist());

    module_->send(dataFlowStart);
}

void Read::sendFinalResponse()
{
    // Construct the final response
    spfsReadResponse* resp = new spfsReadResponse(
        0, SPFS_READ_RESPONSE);
    resp->setContextPointer(readReq_);
    resp->setByteLength(4);
    module_->send(resp);
}

void Read::finish()
{
    // Set the flag so that the originating request is cleaned up during
    // object destruction.  Don't simply delete it because the state is
    // updated after this call to finish()
    cleanupRequest_ = true;
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
