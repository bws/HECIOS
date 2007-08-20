
#include <cassert>
#include <omnetpp.h>
#include "create.h"
#include "fs_server.h"
#include "pvfs_proto_m.h"
using namespace std;

Create::Create(FSServer* module, spfsCreateRequest* createReq)
    : module_(module),
      createReq_(createReq)
{
}

void Create::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState;// = createReq_->getState();

    // Server create states
    enum {
        INIT = 0,
        FINISH = FSM_Steady(1),
    };

    FSM_Switch(currentState)
    {
        case FSM_Enter(INIT):
        {
            assert(0 != dynamic_cast<spfsCreateRequest*>(msg));
            enterFinish();
            break;
        }
    }
}

void Create::enterFinish()
{
    spfsCreateResponse* resp = new spfsCreateResponse(0, SPFS_CREATE_RESPONSE);
    resp->setContextPointer(createReq_);
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
