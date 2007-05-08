
#include <cassert>
#include <omnetpp.h>
#include "create.h"
#include "pvfs_proto_m.h"
using namespace std;

Create::Create(spfsCreateRequest* createReq)
    : createReq_(createReq)
{
}

cMessage* Create::handleServerMessage(cMessage* msg)
{
    // Message response
    cMessage* resp;
    
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
            resp = enterFinish();
            break;
        }
    }

    return resp;
}

cMessage* Create::enterFinish()
{
    spfsCreateResponse* resp = new spfsCreateResponse(0, SPFS_CREATE_RESPONSE);
    resp->setContextPointer(createReq_->contextPointer());
    return resp;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
