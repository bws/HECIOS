#include "fs_read.h"
#include <iostream>
#define FSM_DEBUG  // Enable FSM Debug output
#include <omnetpp.h>
#include "fs_module.h"
#include "mpiio_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSRead::FSRead(fsModule* module, spfsMPIFileReadAtRequest* readReq)
    : fsModule_(module),
      readReq_(readReq)
{
    assert(0 != fsModule_);
    assert(0 != readReq_);
}

// Processing that occurs upon receipt of an MPI-IO Open request
void FSRead::handleMessage(cMessage* msg)
{
    /** Restore the existing state for this Open Request */
    cFSM currentState = readReq_->getState();

    /** File system open state machine states */
    enum {
        INIT = 0,
        READ = FSM_Steady(1),
        COUNT_RESPONSES = FSM_Steady(2),
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
            enterRead();
            break;
        }
        case FSM_Exit(READ):
        {
            //assert(0 != dynamic_cast<spfsReadResponse*>(msg));
            FSM_Goto(currentState, COUNT_RESPONSES);
            break;
        }
        case FSM_Exit(COUNT_RESPONSES):
        {
            assert(0 != dynamic_cast<spfsReadResponse*>(msg));
            bool hasReceivedAllResponses;
            enterCountResponses(hasReceivedAllResponses);
            if (hasReceivedAllResponses)
            {
                cerr << "All responses recv'd for a read" << endl;
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
            enterFinish();
            break;
        }
    }

    // Store current state
    readReq_->setState(currentState);
}

void FSRead::enterRead()
{
    // Construct the template read request
    spfsReadRequest read("ReadStuff", SPFS_READ_REQUEST);
    FSOpenFile* filedes = (FSOpenFile*)readReq_->getFiledes();
    read.setContextPointer(readReq_);
    read.setServerCnt(filedes->metaData->dataHandles.size());
    read.setOffset(filedes->filePtr);
    read.setCount(readReq_->getCount());
    read.setDtype(readReq_->getDtype());

    // Send request to each server
    cerr << "Generating " << read.getServerCnt() << " read reqs\n";
    for (int i = 0; i < read.getServerCnt(); i++)
    {
        spfsReadRequest* req = static_cast<spfsReadRequest*>(read.dup());
        req->setHandle(filedes->metaData->dataHandles[i]);
        fsModule_->send(req, fsModule_->fsNetOut);
    }

    // Set the number of outstanding responses
    readReq_->setResponses(read.getServerCnt());
}

void FSRead::enterCountResponses(bool& outHasReceivedAllResponses)
{
    outHasReceivedAllResponses = false;

    int numOutstandingResponses = readReq_->getResponses();
    readReq_->setResponses(--numOutstandingResponses);

    if (0 == numOutstandingResponses)
    {
        outHasReceivedAllResponses = true;
    }
}

void FSRead::enterFinish()
{
    spfsMPIFileReadAtResponse* mpiResp =
        new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
    mpiResp->setContextPointer(readReq_);
    mpiResp->setIsSuccessful(true);
    mpiResp->setBytesRead(0);  // FIXME
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
