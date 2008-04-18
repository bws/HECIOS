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
#include "fs_lookup_name_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSLookupNameSM::FSLookupNameSM(const Filename& lookupName,
                               spfsMPIRequest* mpiRequest,
                               FSClient* client)
    : lookupName_(lookupName),
      mpiReq_(mpiRequest),
      client_(client)
{
    assert(0 != client_);
    assert(0 != mpiReq_);
}

bool FSLookupNameSM::updateState(cFSM& currentState, cMessage* msg)
{
    // File system lookup name state machine states
    enum {
        INIT = 0,
        LOOKUP_HANDLE = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            bool isCached = isNameCached();
            if (isCached)
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, LOOKUP_HANDLE);
            }
            break;
        }
        case FSM_Enter(LOOKUP_HANDLE):
        {
            lookupHandleOnServer();
            break;
        }
        case FSM_Exit(LOOKUP_HANDLE):
        {
            assert(0 != dynamic_cast<spfsLookupPathResponse*>(msg));
            FSLookupStatus status = processLookup(
                static_cast<spfsLookupPathResponse*>(msg));
            if (SPFS_FOUND == status)
                FSM_Goto(currentState, FINISH);
            else if (SPFS_NOTFOUND == status)
                FSM_Goto(currentState, FINISH);
            else if (SPFS_PARTIAL == status)
                FSM_Goto(currentState, LOOKUP_HANDLE);
            else
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "ERROR: Name does not exist." << endl;
            }
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

bool FSLookupNameSM::isNameCached()
{
    // If the name is the root, it is well known
    if (1 == lookupName_.getNumPathSegments())
    {
        return true;
    }
    
    FSHandle* lookup = client_->fsState().lookupName(lookupName_.str());
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSLookupNameSM::lookupHandleOnServer()
{
    // Find the first resolved handle
    int numResolvedSegments = mpiReq_->getNumResolvedSegments();
    Filename resolvedName = lookupName_.getSegment(numResolvedSegments - 1);

    // Determine the handle of the resolved name
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(resolvedName)->handle;
    
    // Create the lookup request
    cerr << "Lookup: " << lookupName_ << endl;
    cerr << "Resolved so far: " << resolvedName << " " << resolvedHandle << endl;
    spfsLookupPathRequest* req = FSClient::createLookupPathRequest(
        lookupName_, resolvedHandle, numResolvedSegments);
    req->setContextPointer(mpiReq_);
    
    // Send the request
    client_->send(req, client_->getNetOutGate());
}

FSLookupStatus FSLookupNameSM::processLookup(spfsLookupPathResponse* lookupResponse)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());

    // Add the lookup results
    int numResolvedSegments = lookupResponse->getNumResolvedSegments();
    mpiReq_->setNumResolvedSegments(numResolvedSegments);
    
    // Determine lookup results
    FSLookupStatus lookupStatus = lookupResponse->getStatus();
    if (SPFS_FOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename resolvedName = lookupName_.getSegment(numResolvedSegments - 1);
        const FSMetaData* meta =
            FileBuilder::instance().getMetaData(resolvedName);
        client_->fsState().insertName(resolvedName.str(), meta->handle);
    }
    return lookupStatus;
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
