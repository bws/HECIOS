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
#include "fs_update_time.h"
#include <cassert>
#include <iostream>
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSUpdateTime::FSUpdateTime(FSClient* client,
                           spfsMPIFileUpdateTimeRequest* utimeReq)
    : client_(client),
      utimeReq_(utimeReq)
{
    assert(0 != client_);
    assert(0 != utimeReq_);
}

// Processing that occurs upon receipt of an MPI-IO UpdateTime request
void FSUpdateTime::handleMessage(cMessage* msg)
{
    /** Restore the existing state for this request */
    cFSM currentState = utimeReq_->getState();

    /** File system update time state machine states */
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
    utimeReq_->setState(currentState);
}

bool FSUpdateTime::isParentNameCached()
{
    // Lookup the parent directory in the name cache
    Filename openFile(utimeReq_->getFileName());
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

bool FSUpdateTime::isParentAttrCached()
{
    // Lookup the parent directory in the name cache
    Filename utimeFile(utimeReq_->getFileName());
    Filename parentDir = utimeFile.getParent();
    
    FSHandle* lookup = client_->fsState().lookupName(parentDir.str());
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSUpdateTime::lookupParentOnServer()
{
    // Find the first resolved handle
    int numResolvedSegments = utimeReq_->getNumResolvedSegments();
    Filename utimeFile(utimeReq_->getFileName());
    Filename parent = utimeFile.getParent();
    Filename resolvedName = parent.getSegment(numResolvedSegments - 1);

    // Determine the handle of the resolved name
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(resolvedName)->handle;
    
    // Create the lookup request
    cerr << "UTime: " << utimeFile << endl;
    cerr << "Client looking up: " << resolvedName << " " << resolvedHandle << endl;
    spfsLookupPathRequest* req = FSClient::createLookupPathRequest(
        parent, resolvedHandle, numResolvedSegments);
    req->setContextPointer(utimeReq_);

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

void FSUpdateTime::getParentAttributes()
{
    Filename utimeName(utimeReq_->getFileName());
    Filename parent = utimeName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parent);

    // Construct the request
    spfsGetAttrRequest *req =
        FSClient::createGetAttrRequest(parentMeta->handle,
                                       SPFS_METADATA_OBJECT);
    req->setContextPointer(utimeReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSUpdateTime::cacheParentAttributes()
{
    Filename utimeName(utimeReq_->getFileName());
    Filename parentName =  utimeName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    client_->fsState().insertAttr(parentMeta->handle, *parentMeta);
}

void FSUpdateTime::lookupNameOnServer()
{
    // Find the first resolved handle
    Filename utimeFile(utimeReq_->getFileName());
    Filename parent = utimeFile.getParent();

    // Determine the handle of the parent
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(parent)->handle;
    
    // Create the lookup request
    spfsLookupPathRequest* req = FSClient::createLookupPathRequest(
        utimeFile, resolvedHandle, parent.getNumPathSegments());
    req->setContextPointer(utimeReq_);

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

FSLookupStatus FSUpdateTime::processLookup(
    spfsLookupPathResponse* lookupResponse)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());

    // Add the lookup results
    int numResolvedSegments = lookupResponse->getNumResolvedSegments();
    utimeReq_->setNumResolvedSegments(numResolvedSegments);
    
    // Determine lookup results
    FSLookupStatus lookupStatus = lookupResponse->getStatus();
    if (SPFS_FOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename utimeFile(utimeReq_->getFileName());
        Filename resolvedName = utimeFile.getSegment(numResolvedSegments - 1);
        const FSMetaData* meta =
            FileBuilder::instance().getMetaData(resolvedName);
        cerr << "Adding name to cache: " << resolvedName << endl;
        client_->fsState().insertName(resolvedName.str(), meta->handle);
    }
    else if (SPFS_NOTFOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename utimeFile(utimeReq_->getFileName());
        Filename resolvedName = utimeFile.getSegment(numResolvedSegments - 1);
        cerr << "Could not lookup: " << utimeFile << " unable to resolve: "
             << resolvedName << endl;
    }
    return lookupStatus;
}

void FSUpdateTime::writeAttributes()
{
    Filename utimeName(utimeReq_->getFileName());
    const FSMetaData* meta = FileBuilder::instance().getMetaData(utimeName);
    spfsSetAttrRequest *req =
        FSClient::createSetAttrRequest(meta->handle, SPFS_METADATA_OBJECT);
    req->setContextPointer(utimeReq_);
    client_->send(req, client_->getNetOutGate());

    // Add the attributes to the cache
    client_->fsState().insertAttr(meta->handle, *meta);
}

void FSUpdateTime::finish()
{
    spfsMPIFileUpdateTimeResponse* mpiResp =
        new spfsMPIFileUpdateTimeResponse(0,
                                          SPFS_MPI_FILE_UPDATE_TIME_RESPONSE);
    mpiResp->setContextPointer(utimeReq_);
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
