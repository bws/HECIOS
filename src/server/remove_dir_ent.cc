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
#include "remove_dir_ent.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

RemoveDirEnt::RemoveDirEnt(FSServer* module,
                           spfsRemoveDirEntRequest* removeDirEntReq)
    : module_(module),
      removeDirEntReq_(removeDirEntReq)
{
}

void RemoveDirEnt::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = removeDirEntReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        WRITE_DIR_ENT = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            module_->recordRemoveDirEnt();
            FSM_Goto(currentState, WRITE_DIR_ENT);
            break;
        }
        case FSM_Enter(WRITE_DIR_ENT):
        {
            assert(0 != dynamic_cast<spfsRemoveDirEntRequest*>(msg));
            writeDirEnt();
            break;
        }
        case FSM_Exit(WRITE_DIR_ENT):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileWriteResponse*>(msg));
            module_->recordRemoveDirEntDiskDelay(msg);
            finish();
            break;
        }
    }

    // Store current state
    removeDirEntReq_->setState(currentState);
}

void RemoveDirEnt::writeDirEnt()
{
    // Convert the handle into a local file name
    Filename filename(removeDirEntReq_->getHandle());

    // Create the file write request
    spfsOSFileWriteRequest* fileWrite = new spfsOSFileWriteRequest();
    fileWrite->setContextPointer(removeDirEntReq_);
    fileWrite->setFilename(filename.c_str());
    fileWrite->setOffsetArraySize(1);
    fileWrite->setExtentArraySize(1);
    fileWrite->setOffset(0, 0);
    fileWrite->setExtent(0, module_->getDirectoryEntrySize());

    // Send the write request
    module_->send(fileWrite);
}

void RemoveDirEnt::finish()
{
    spfsRemoveDirEntResponse* resp =
        new spfsRemoveDirEntResponse(0, SPFS_REMOVE_DIR_ENT_RESPONSE);
    resp->setContextPointer(removeDirEntReq_);
    resp->setByteLength(4);
    module_->sendDelayed(resp, FSServer::removeDirEntProcessingDelay());
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
