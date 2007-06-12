
#include <cassert>
#include <omnetpp.h>
#include "lookup.h"
#include "pvfs_proto_m.h"
using namespace std;

Lookup::Lookup(spfsLookupPathRequest* lookupReq)
    : lookupReq_(lookupReq)
{
}

cMessage* Lookup::handleServerMessage(cMessage* msg)
{
    // Message response
    cMessage* resp;
    
    // Restore the existing state for this request
    cFSM currentState;// = lookupReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        FINISH = FSM_Steady(1),
    };

    FSM_Switch(currentState)
    {
        case FSM_Enter(INIT):
        {
            assert(0 != dynamic_cast<spfsLookupPathRequest*>(msg));
            resp = enterFinish();
            break;
        }
    }

    return resp;
}

cMessage* Lookup::enterFinish()
{
    spfsLookupPathResponse* resp = new spfsLookupPathResponse(
        0, SPFS_LOOKUP_PATH_RESPONSE);
    resp->setContextPointer(lookupReq_->contextPointer());
    resp->setSocketId(lookupReq_->getSocketId());
    resp->setStatus(SPFS_FOUND);
    resp->setHandleCount(1);
    resp->setAttrCount(0);
    resp->setHandlesArraySize(1);
    resp->setHandles(0, 1200);

    // Cleanup initiating request
    delete lookupReq_;
    lookupReq_ = 0;

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
