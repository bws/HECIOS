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
#include "create.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

Create::Create(FSServer* module, spfsCreateRequest* createReq)
    : module_(module),
      createReq_(createReq)
{
}

void Create::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = createReq_->getState();

    // Server create states
    enum {
        INIT = 0,
        CREATE = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
    case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsCreateRequest*>(msg));
            FSM_Goto(currentState, CREATE);
            break;
        }
    case FSM_Enter(CREATE):
        {
            assert(0 != dynamic_cast<spfsCreateRequest*>(msg));
            enterCreate();
            break;
        }
    case FSM_Exit(CREATE):
        {
            assert(0 != dynamic_cast<spfsOSFileOpenResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
    case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileOpenResponse*>(msg));
            enterFinish();
            break;
        }
    }

    // Store the state in the request
    createReq_->setState(currentState);
}

void Create::enterCreate()
{
    spfsOSFileOpenRequest* openRequest =
        new spfsOSFileOpenRequest(0, SPFS_OS_FILE_OPEN_REQUEST);
    openRequest->setContextPointer(createReq_);

    // Extract the handle as the file name
    Filename f(createReq_->getHandle());
    openRequest->setFilename(f.c_str());
    openRequest->setIsCreate(true);
    
    // Send the request to the storage layer
    module_->send(openRequest);
}

void Create::enterFinish()
{
    spfsCreateResponse* resp = new spfsCreateResponse(0, SPFS_CREATE_RESPONSE);
    resp->setContextPointer(createReq_);
    resp->setByteLength(4);
    module_->send(resp);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab cino=g0:
 */
