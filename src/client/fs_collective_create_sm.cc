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
