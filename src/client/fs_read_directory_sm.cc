//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
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
#include "fs_read_directory_sm.h"
#include <omnetpp.h>
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSReadDirectorySM::FSReadDirectorySM(FileDescriptor* fd,
                                     size_t numEntries,
                                     spfsMPIRequest* mpiReq,
                                     FSClient* client)
    : descriptor_(fd),
      numEntries_(numEntries),
      mpiReq_(mpiReq),
      client_(client)
{
    assert(0 != descriptor_);
    assert(numEntries_ > 0);
    assert(0 != mpiReq_);
    assert(0 != client_);
}

bool FSReadDirectorySM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system read directory state machine states */
    enum {
        INIT = 0,
        READ_DIR_ENTS = FSM_Steady(1),
        FINISH = FSM_Steady(2)
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, READ_DIR_ENTS);
            break;
        }
        case FSM_Enter(READ_DIR_ENTS):
        {
            readDirEnt();
            break;
        }
        case FSM_Exit(READ_DIR_ENTS):
        {
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

void FSReadDirectorySM::readDirEnt()
{
    const FSMetaData* meta = descriptor_->getMetaData();
    spfsReadDirRequest* req =
        FSClient::createReadDirRequest(meta->dataHandles[0], numEntries_);
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
