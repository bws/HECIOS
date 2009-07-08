//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
            module_->recordWrite();
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
    spfsServerDataFlowStart* dataFlowStart =
        new spfsServerDataFlowStart(0, SPFS_DATA_FLOW_START);
    dataFlowStart->setContextPointer(writeReq_);
    dataFlowStart->setMetaHandle(writeReq_->getMetaHandle());

    // Set the flow configuration
    dataFlowStart->setFlowType(DataFlow::SERVER_FLOW_TYPE);
    dataFlowStart->setFlowMode(DataFlow::WRITE_MODE);

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

    // Server data flow specific information
    dataFlowStart->setMetaHandle(writeReq_->getMetaHandle());

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
