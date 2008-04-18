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
#include "fs_collective_get_attributes_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSCollectiveGetAttributesSM::FSCollectiveGetAttributesSM(
    const Filename& filename,
    bool calculateSize,
    spfsMPIRequest* mpiRequest,
    FSClient* client)
    : handle_(FileBuilder::instance().getMetaData(filename)->handle),
      calculateSize_(calculateSize),
      mpiReq_(mpiRequest),
      client_(client)
{
    assert(0 != client_);
    assert(0 != mpiReq_);
    assert(true == calculateSize_);
}

bool FSCollectiveGetAttributesSM::updateState(cFSM& currentState, cMessage* msg)
{
    // File system get attributes state machine states
    enum {
        INIT = 0,
        COLLECTIVE_GET_ATTR = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            bool isCached = isAttrCached();
            if (isCached && !calculateSize_)
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COLLECTIVE_GET_ATTR);
            }
            break;
        }
        case FSM_Enter(COLLECTIVE_GET_ATTR):
        {
            getAttributesCollective();
            break;
        }
        case FSM_Exit(COLLECTIVE_GET_ATTR):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            cacheAttributes();
            isComplete = true;
            break;
        }
    }
    return isComplete;
}

bool FSCollectiveGetAttributesSM::isAttrCached()
{
    FSMetaData* lookup = client_->fsState().lookupAttr(handle_);
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSCollectiveGetAttributesSM::getAttributesCollective()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(handle_);
    spfsCollectiveGetAttrRequest* req =
        FSClient::createCollectiveGetAttrRequest(meta->handle,
                                                 meta->dataHandles);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSCollectiveGetAttributesSM::cacheAttributes()
{
    const FSMetaData* attr = FileBuilder::instance().getMetaData(handle_);
    client_->fsState().insertAttr(handle_, *attr);
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
