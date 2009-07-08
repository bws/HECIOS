//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_collective_remove_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSCollectiveRemoveSM::FSCollectiveRemoveSM(const Filename& filename,
                                           spfsMPIRequest* mpiReq,
                                           FSClient* client)
    : removeFilename_(filename),
      mpiReq_(mpiReq),
      client_(client)
{
}

bool FSCollectiveRemoveSM::updateState(cFSM& currentState, cMessage* msg)
{
    // File system collective create state machine states
    enum {
        INIT = 0,
        COLLECTIVE_REMOVE = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, COLLECTIVE_REMOVE);
            break;
        }
        case FSM_Enter(COLLECTIVE_REMOVE):
        {
            collectiveRemove();
            break;
        }
        case FSM_Exit(COLLECTIVE_REMOVE):
        {
            assert(0 != dynamic_cast<spfsCollectiveRemoveResponse*>(msg));
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

void FSCollectiveRemoveSM::collectiveRemove()
{
    // Get the metadata handle
    Filename parentName = removeFilename_.getParent();
    FSMetaData* meta = FileBuilder::instance().getMetaData(removeFilename_);
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    assert(0 != meta);
    assert(0 != parentMeta);

    spfsCollectiveRemoveRequest* req = FSClient::createCollectiveRemoveRequest(
        parentMeta->dataHandles[0], meta->handle, meta->dataHandles);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());

    // Set the file size to 0
    for (size_t i = 0; i < meta->bstreamSizes.size(); i++)
    {
        meta->bstreamSizes[i] = 0;
    }
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
