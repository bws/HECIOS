#include "fs_write.h"
#include <cassert>
#include <iostream>
#define FSM_DEBUG  // Enable FSM Debug output
#include <omnetpp.h>
#include "fs_module.h"
#include "mpiio_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSWrite::FSWrite(fsModule* module, spfsMPIFileWriteRequest* writeReq)
    : fsModule_(module),
      writeReq_(writeReq)
{
    assert(0 != fsModule_);
    assert(0 != writeReq_);
}

// Processing that occurs upon receipt of an MPI-IO Write request
void FSWrite::handleMessage(cMessage* msg)
{
    /** Restore the existing state for this request */
    cFSM currentState = writeReq_->getState();

    /** File system open state machine states */
    enum {
        INIT = 0,
        WRITE = FSM_Steady(1),
        COUNT_RESPONSES = FSM_Steady(2),
        FINISH = FSM_Steady(3),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileWriteRequest*>(msg));
            FSM_Goto(currentState, WRITE);
            break;
        }
        case FSM_Enter(WRITE):
        {
            enterWrite();
            break;
        }
        case FSM_Exit(WRITE):
        {
            assert(0 != dynamic_cast<spfsWriteResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Exit(COUNT_RESPONSES):
        {
            assert(0 != dynamic_cast<spfsWriteResponse*>(msg));
            bool hasReceivedAllResponses;
            enterCountResponses(hasReceivedAllResponses);
            if (hasReceivedAllResponses)
            {
                cerr << "All responses recv'd for a write" << endl;
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT_RESPONSES);
            }
            
            // Cleanup responses
            delete msg;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsWriteResponse*>(msg));
            enterFinish(static_cast<spfsWriteResponse*>(msg));
            break;
        }
    }

    // Store current state
    writeReq_->setState(currentState);
}

void FSWrite::enterWrite()
{
    FSOpenFile* filedes = (FSOpenFile*)writeReq_->getFileDes();
    assert(0 != filedes);
    
    // Construct the server write request
    spfsWriteRequest write(0, SPFS_WRITE_REQUEST);
    write.setContextPointer(writeReq_);
    write.setServerCnt(filedes->metaData->dataHandles.size());
    write.setOffset(filedes->filePtr);
    write.setCount(writeReq_->getCount());
    write.setDtype(writeReq_->getDataType());

    // Send request to each server
    for (size_t i = 0; i < filedes->metaData->dataHandles.size(); i++)
    {
        spfsWriteRequest* req = static_cast<spfsWriteRequest*>(write.dup());
        req->setHandle(filedes->metaData->dataHandles[i]);
        fsModule_->send(req, fsModule_->fsNetOut);
    }

    // Set the number of responses
    writeReq_->setResponses(filedes->metaData->dataHandles.size());
}

void FSWrite::enterCountResponses(bool& outHasReceivedAllResponses)
{
    outHasReceivedAllResponses = false;

    int numOutstandingResponses = writeReq_->getResponses();
    writeReq_->setResponses(--numOutstandingResponses);

    if (0 == numOutstandingResponses)
    {
        outHasReceivedAllResponses = true;
    }
}

void FSWrite::enterFinish(spfsWriteResponse* writeResp)
{
    spfsMPIFileWriteAtResponse* mpiResp =
        new spfsMPIFileWriteAtResponse(0, SPFS_MPI_FILE_WRITE_AT_RESPONSE);
    mpiResp->setContextPointer(writeReq_);
    mpiResp->setIsSuccessful(true);
    fsModule_->send(mpiResp, fsModule_->fsMpiOut);
}
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
