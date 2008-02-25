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
#include "fs_stat.h"
#include <cassert>
#include <iostream>
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSStat::FSStat(FSClient* client, spfsMPIFileUpdateTimeRequest* statReq)
    : client_(client),
      statReq_(statReq)
{
    assert(0 != client_);
    assert(0 != statReq_);
}

// Processing that occurs upon receipt of an MPI-IO UpdateTime request
void FSStat::handleMessage(cMessage* msg)
{
    // TODO
    cerr << "ERROR: File STAT is not yet implemented correctly!!!" << endl;
    /** Restore the existing state for this request */
    cFSM currentState = statReq_->getState();

    /** File system stat state machine states */
    enum {
        INIT = 0,
        LOOKUP_PARENT_HANDLE = FSM_Steady(1),
        GET_PARENT_ATTRIBUTES = FSM_Steady(2),
        LOOKUP_NAME = FSM_Steady(4),
        WRITE_ATTR = FSM_Steady(5),
        FINISH = FSM_Steady(10)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileUpdateTimeRequest*>(msg));
            bool nameCached = isParentNameCached();
            if (nameCached)
            {
                bool attrCached = isParentAttrCached();
                if (attrCached)
                {
                    FSM_Goto(currentState, LOOKUP_NAME);
                }
                else
                {
                    FSM_Goto(currentState, GET_PARENT_ATTRIBUTES);
                }
            }
            else
            {
                FSM_Goto(currentState, LOOKUP_PARENT_HANDLE);
            }
            break;
        }
        case FSM_Enter(LOOKUP_PARENT_HANDLE):
        {
            lookupParentOnServer();
            break;
        }
        case FSM_Exit(LOOKUP_PARENT_HANDLE):
        {
            assert(0 != dynamic_cast<spfsLookupPathResponse*>(msg));
            FSLookupStatus status = processLookup(
                static_cast<spfsLookupPathResponse*>(msg));
            if (SPFS_FOUND == status)
                FSM_Goto(currentState, GET_PARENT_ATTRIBUTES);
            else if (SPFS_PARTIAL == status)
                FSM_Goto(currentState, LOOKUP_PARENT_HANDLE);
            else if (SPFS_NOTFOUND == status)
                FSM_Goto(currentState, FINISH);
            else
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "ERROR: Dir does not exist during creation." << endl;
            }
            break;
        }
        case FSM_Enter(GET_PARENT_ATTRIBUTES):
        {
            getParentAttributes();
            break;
        }
        case FSM_Exit(GET_PARENT_ATTRIBUTES):
        {
            cacheParentAttributes();
            FSM_Goto(currentState, LOOKUP_NAME);
            break;
        }
        case FSM_Enter(LOOKUP_NAME):
        {
            lookupNameOnServer();
            break;
        }
        case FSM_Exit(LOOKUP_NAME):
        {
            FSLookupStatus status = processLookup(
                static_cast<spfsLookupPathResponse*>(msg));
            assert(SPFS_FOUND == status);
            FSM_Goto(currentState, WRITE_ATTR);
            break;
        }
        case FSM_Enter(WRITE_ATTR):
        {
            writeAttributes();
            break;
        }
        case FSM_Exit(WRITE_ATTR):
        {
            assert(0 != dynamic_cast<spfsSetAttrResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            finish();
            break;
        }
    }

    // Store current state
    statReq_->setState(currentState);
}

bool FSStat::isParentNameCached()
{
    // Lookup the parent directory in the name cache
    Filename openFile(statReq_->getFileName());
    Filename parentDir = openFile.getParent();

    // If the parent directory is the root, it is well known
    if (1 == parentDir.getNumPathSegments())
    {
        return true;
    }
    
    FSHandle* lookup = client_->fsState().lookupName(parentDir.str());
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

bool FSStat::isParentAttrCached()
{
    // Lookup the parent directory in the name cache
    Filename statFile(statReq_->getFileName());
    Filename parentDir = statFile.getParent();
    
    FSHandle* lookup = client_->fsState().lookupName(parentDir.str());
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSStat::lookupParentOnServer()
{
    // Find the first resolved handle
    int numResolvedSegments = statReq_->getNumResolvedSegments();
    Filename statFile(statReq_->getFileName());
    Filename parent = statFile.getParent();
    Filename resolvedName = parent.getSegment(numResolvedSegments - 1);

    // Determine the handle of the resolved name
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(resolvedName)->handle;
    
    // Create the lookup request
    cerr << "Stat: " << statFile << endl;
    cerr << "Client looking up: " << resolvedName << " " << resolvedHandle << endl;
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(statReq_);
    req->setFilename(parent.c_str());
    req->setHandle(resolvedHandle);
    req->setNumResolvedSegments(numResolvedSegments);

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

void FSStat::getParentAttributes()
{
    Filename statName(statReq_->getFileName());
    Filename parent = statName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parent);

    // Construct the request
    spfsGetAttrRequest *req = new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    req->setContextPointer(statReq_);
    req->setHandle(parentMeta->handle);
    client_->send(req, client_->getNetOutGate());
}

void FSStat::cacheParentAttributes()
{
    Filename statName(statReq_->getFileName());
    Filename parentName =  statName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    client_->fsState().insertAttr(parentMeta->handle, *parentMeta);
}

void FSStat::lookupNameOnServer()
{
    // Find the first resolved handle
    Filename statFile(statReq_->getFileName());
    Filename parent = statFile.getParent();

    // Determine the handle of the parent
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(parent)->handle;
    
    // Create the lookup request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(statReq_);
    req->setFilename(statFile.c_str());
    req->setHandle(resolvedHandle);
    req->setNumResolvedSegments(parent.getNumPathSegments());

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

FSLookupStatus FSStat::processLookup(spfsLookupPathResponse* lookupResponse)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());

    // Add the lookup results
    int numResolvedSegments = lookupResponse->getNumResolvedSegments();
    statReq_->setNumResolvedSegments(numResolvedSegments);
    
    // Determine lookup results
    FSLookupStatus lookupStatus = lookupResponse->getStatus();
    if (SPFS_FOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename statFile(statReq_->getFileName());
        Filename resolvedName = statFile.getSegment(numResolvedSegments - 1);
        const FSMetaData* meta =
            FileBuilder::instance().getMetaData(resolvedName);
        cerr << "Adding name to cache: " << resolvedName << endl;
        client_->fsState().insertName(resolvedName.str(), meta->handle);
    }
    else if (SPFS_NOTFOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename statFile(statReq_->getFileName());
        Filename resolvedName = statFile.getSegment(numResolvedSegments - 1);
        cerr << "Could not lookup: " << statFile << " unable to resolve: "
             << resolvedName << endl;
    }
    return lookupStatus;
}

void FSStat::writeAttributes()
{
    Filename statName(statReq_->getFileName());
    const FSMetaData* meta = FileBuilder::instance().getMetaData(statName);
    spfsSetAttrRequest *req = new spfsSetAttrRequest(0, SPFS_SET_ATTR_REQUEST);
    req->setContextPointer(statReq_);
    req->setHandle(meta->handle);
    req->setByteLength(8 + 8 + 64);
    client_->send(req, client_->getNetOutGate());

    // Add the attributes to the cache
    //addAttributesToCache();
}

void FSStat::finish()
{
    spfsMPIFileUpdateTimeResponse* mpiResp =
        new spfsMPIFileUpdateTimeResponse(0,
                                          SPFS_MPI_FILE_UPDATE_TIME_RESPONSE);
    mpiResp->setContextPointer(statReq_);
    mpiResp->setIsSuccessful(true);
    client_->send(mpiResp, client_->getAppOutGate());
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
