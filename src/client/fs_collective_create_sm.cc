//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_collective_create_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSCollectiveCreateSM::FSCollectiveCreateSM(const Filename& filename,
                                           spfsMPIRequest* mpiReq,
                                           FSClient* client)
    : createFilename_(filename),
      mpiReq_(mpiReq),
      client_(client)
{
}

bool FSCollectiveCreateSM::updateState(cFSM& currentState, cMessage* msg)
{
    // File system collective create state machine states
    enum {
        INIT = 0,
        COLLECTIVE_CREATE = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, COLLECTIVE_CREATE);
            break;
        }
        case FSM_Enter(COLLECTIVE_CREATE):
        {
            collectiveCreate();
            break;
        }
        case FSM_Exit(COLLECTIVE_CREATE):
        {
            assert(0 != dynamic_cast<spfsCollectiveCreateResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            isComplete = true;
            break;
        }
    }

    return isComplete;
}

void FSCollectiveCreateSM::collectiveCreate()
{
    // Get the file and it's parent metadata
    FSMetaData* meta = FileBuilder::instance().getMetaData(createFilename_);
    Filename parent = createFilename_.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parent);
    assert(0 != meta);
    assert(0 != parentMeta);

    spfsCollectiveCreateRequest* req = FSClient::createCollectiveCreateRequest(
        parentMeta->dataHandles[0], meta->handle, meta->dataHandles);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
