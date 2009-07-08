//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
            module_->recordRead();
            if (0 != readReq_->getLocalSize())
            {
                FSM_Goto(currentState, START_DATA_FLOW);
            }
            else
            {
                FSM_Goto(currentState, SEND_FINAL_RESPONSE);
            }
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
    spfsServerDataFlowStart* dataFlowStart =
        new spfsServerDataFlowStart(0, SPFS_DATA_FLOW_START);
    dataFlowStart->setContextPointer(readReq_);
    dataFlowStart->setMetaHandle(readReq_->getMetaHandle());

    // Set the flow configuration
    dataFlowStart->setFlowType(DataFlow::SERVER_FLOW_TYPE);
    dataFlowStart->setFlowMode(DataFlow::READ_MODE);

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
    dataFlowStart->setBstreamSize(readReq_->getBstreamSize());

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
    // If a flow was initiated for this request
    if (0 != readReq_->getLocalSize())
    {
        // Set the flag so that the originating request is cleaned up during
        // object destruction.  Don't simply delete it because the state is
        // updated after this call to finish()
        cleanupRequest_ = true;
    }
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
