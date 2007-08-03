
#include "read.h"
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "data_type_processor.h"
#include "file_distribution.h"
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
    // Calculate the response size
    FileLayout layout;
    DataTypeProcessor::createFileLayoutForServer(readReq_->getOffset(),
                                                 readReq_->getDataType(),
                                                 readReq_->getCount(),
                                                 *(readReq_->getDist()),
                                                 10000000,
                                                 layout);

    // Sum all the extents to determine total write size
    size_t reqBytes = accumulate(layout.extents.begin(),
                                 layout.extents.end(), 0);
    assert(0 != reqBytes);
    
    spfsReadResponse* resp = new spfsReadResponse(
        0, SPFS_READ_RESPONSE);
    resp->setByteLength(reqBytes);    
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
