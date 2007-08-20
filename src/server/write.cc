
#include "write.h"
#include <cassert>
#include <omnetpp.h>
#include "fs_server.h"
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
        FINISH = FSM_Steady(1),
    };

    FSM_Switch(currentState)
    {
        case FSM_Enter(INIT):
        {
            assert(0 != dynamic_cast<spfsWriteRequest*>(msg));
            enterFinish();
            break;
        }
    }
}

void Write::enterFinish()
{
    spfsWriteResponse* resp = new spfsWriteResponse(
        0, SPFS_WRITE_RESPONSE);
    resp->setContextPointer(writeReq_);
    module_->send(resp, "netOut");
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
