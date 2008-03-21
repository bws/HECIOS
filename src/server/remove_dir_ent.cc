//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
            assert(0 != dynamic_cast<spfsCreateDirEntRequest*>(msg));
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
