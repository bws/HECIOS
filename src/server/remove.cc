//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include <omnetpp.h>
#include "remove.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

Remove::Remove(FSServer* module, spfsRemoveRequest* removeReq)
    : module_(module),
      removeReq_(removeReq)
{
}

void Remove::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = removeReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        REMOVE_OBJECT = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            module_->recordRemoveObject();
            FSM_Goto(currentState, REMOVE_OBJECT);
            break;
        }
        case FSM_Enter(REMOVE_OBJECT):
        {
            assert(0 != dynamic_cast<spfsRemoveRequest*>(msg));
            unlinkFile();
            break;
        }
        case FSM_Exit(REMOVE_OBJECT):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileUnlinkResponse*>(msg));
            module_->recordRemoveObjectDiskDelay(msg);
            finish();
            break;
        }
    }

    // Store current state
    removeReq_->setState(currentState);
}

void Remove::unlinkFile()
{
    // Convert the handle into a local file name
    Filename filename(removeReq_->getHandle());

    // Create the file unlink request
    spfsOSFileUnlinkRequest* fileUnlink = new spfsOSFileUnlinkRequest();
    fileUnlink->setContextPointer(removeReq_);
    fileUnlink->setFilename(filename.c_str());

    // Send the unlink request
    module_->send(fileUnlink);
}

void Remove::finish()
{

    spfsRemoveResponse* resp = new spfsRemoveResponse(0, SPFS_REMOVE_RESPONSE);
    resp->setContextPointer(removeReq_);
    resp->setByteLength(4);

    simtime_t processingDelay = 0.0;
    if (SPFS_DATA_OBJECT == removeReq_->getObjectType())
    {
        processingDelay = FSServer::removeObjectProcessingDelay();
    }
    else
    {
        assert(SPFS_METADATA_OBJECT == removeReq_->getObjectType());
        processingDelay = FSServer::removeMetaProcessingDelay();
    }
    module_->sendDelayed(resp, processingDelay);
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
