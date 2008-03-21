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
    module_->sendDelayed(resp, FSServer::removeObjectProcessingDelay());
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
