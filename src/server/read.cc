
#include "read.h"
#include <cassert>
#include <omnetpp.h>
#include "pvfs_proto_m.h"
using namespace std;

Read::Read(spfsReadRequest* readReq)
    : readReq_(readReq)
{
}

cMessage* Read::handleServerMessage(cMessage* msg)
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
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            resp = enterFinish();
            break;
        }
    }

    return resp;
}

cMessage* Read::enterFinish()
{
    spfsReadResponse* resp = new spfsReadResponse(
        0, SPFS_READ_RESPONSE);

    // Calculate the response size
    size_t bytes = 8 + readReq_->getCount() * readReq_->getDataType() / 8;
    resp->setByteLength(bytes);
    
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
