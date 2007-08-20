
#include "lookup.h"
#include <cassert>
#include <omnetpp.h>
#include "fs_server.h"
#include "pvfs_proto_m.h"
using namespace std;

Lookup::Lookup(FSServer* module, spfsLookupPathRequest* lookupReq)
    : module_(module),
      lookupReq_(lookupReq)
{
}

void Lookup::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = lookupReq_->getState();

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
            enterFinish();
            break;
        }
    }
}

void Lookup::enterFinish()
{
    spfsLookupPathResponse* resp = new spfsLookupPathResponse(
        0, SPFS_LOOKUP_PATH_RESPONSE);
    resp->setStatus(SPFS_FOUND);
    resp->setHandleCount(1);
    resp->setAttrCount(0);
    resp->setHandlesArraySize(1);
    resp->setHandles(0, 1200);
    resp->setContextPointer(lookupReq_);
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
