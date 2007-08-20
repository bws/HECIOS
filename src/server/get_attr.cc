
#include "get_attr.h"
#include <cassert>
#include <omnetpp.h>
#include "fs_server.h"
#include "pvfs_proto_m.h"
using namespace std;

GetAttr::GetAttr(FSServer* module, spfsGetAttrRequest* getAttrReq)
    : module_(module),
      getAttrReq_(getAttrReq)
{
}

void GetAttr::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = getAttrReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        FINISH = FSM_Steady(1),
    };

    FSM_Switch(currentState)
    {
        case FSM_Enter(INIT):
        {
            assert(0 != dynamic_cast<spfsGetAttrRequest*>(msg));
            enterFinish();
            break;
        }
    }
}

void GetAttr::enterFinish()
{
    spfsGetAttrResponse* resp = new spfsGetAttrResponse(
        0, SPFS_GET_ATTR_RESPONSE);
    resp->setContextPointer(getAttrReq_);
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
