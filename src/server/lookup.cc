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

#include "lookup.h"
#include <cassert>
#include <omnetpp.h>
#include "file_builder.h"
#include "filename.h"
#include "fs_server.h"
#include "pvfs_proto_m.h"
#include "os_proto_m.h"
using namespace std;

Lookup::Lookup(FSServer* module, spfsLookupPathRequest* lookupReq)
    : module_(module),
      lookupReq_(lookupReq)
{
}

void Lookup::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = lookupReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        LOOKUP_NAME = FSM_Steady(1),
        FINISH_COMPLETE_LOOKUP = FSM_Steady(2),
        FINISH_PARTIAL_LOOKUP = FSM_Steady(3)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsLookupPathRequest*>(msg));
            FSM_Goto(currentState, LOOKUP_NAME);
            break;
        }
        case FSM_Enter(LOOKUP_NAME):
        {
            lookupName();
            break;
        }
        case FSM_Exit(LOOKUP_NAME):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            if (lookupIsComplete())
            {
                FSM_Goto(currentState, FINISH_COMPLETE_LOOKUP);
            }
            else if (localLookupIsComplete())
            {
                FSM_Goto(currentState, FINISH_PARTIAL_LOOKUP);
            }
            else
            {
                FSM_Goto(currentState, LOOKUP_NAME);
            }
            break;
        }
        case FSM_Enter(FINISH_COMPLETE_LOOKUP):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            finish(SPFS_FOUND);
            break;
        }
        case FSM_Enter(FINISH_PARTIAL_LOOKUP):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            finish(SPFS_PARTIAL);
            break;
        }
    }
    // Store current state
    lookupReq_->setState(currentState);

}

void Lookup::lookupName()
{
    Filename fullName(lookupReq_->getFilename());
    int nextSegment = lookupReq_->getNumResolvedSegments();

    // Determine the location of the directory entries
    Filename parentName = fullName.getSegment(nextSegment - 1);
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    FSHandle parentHandle = parentMeta->dataHandles[0];

    // Create the directory entry read request
    spfsOSFileReadRequest* fileRead =
        new spfsOSFileReadRequest(0, SPFS_OS_FILE_READ_REQUEST);
    fileRead->setContextPointer(lookupReq_);
    Filename localFilename(parentHandle);    
    fileRead->setFilename(localFilename.c_str());
    fileRead->setOffsetArraySize(1);
    fileRead->setExtentArraySize(1);
    fileRead->setOffset(0, 0);
    fileRead->setExtent(0, parentMeta->size);

    // Send the request
    module_->send(fileRead);

    // Increment the number of resolved segments
    lookupReq_->setNumResolvedSegments(nextSegment + 1);
}

bool Lookup::lookupIsComplete()
{
    // The lookup is complete if the name is fully resolved
    Filename fullName(lookupReq_->getFilename());
    size_t nextSegment = lookupReq_->getNumResolvedSegments();
    if ((nextSegment) == fullName.getNumPathSegments())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Lookup::localLookupIsComplete()
{
    // Determine if the next path segment resides on another server
    Filename fullName(lookupReq_->getFilename());
    size_t nextSegment = lookupReq_->getNumResolvedSegments();
    Filename nextName = fullName.getSegment(nextSegment);
    FSMetaData* nextMeta = FileBuilder::instance().getMetaData(nextName);
    if (module_->handleIsLocal(nextMeta->handle))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Lookup::finish(int lookupStatus)
{
    spfsLookupPathResponse* resp = new spfsLookupPathResponse(
        0, SPFS_LOOKUP_PATH_RESPONSE);
    resp->setContextPointer(lookupReq_);
    resp->setStatus(lookupStatus);
    resp->setNumResolvedSegments(lookupReq_->getNumResolvedSegments());

    // In the case where this full lookup is a create
    // modify the fully complete response
    if (SPFS_FOUND == lookupStatus && lookupReq_->getIsCreate())
    {
        resp->setStatus(SPFS_NOTFOUND);
    }
    
    module_->send(resp);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
