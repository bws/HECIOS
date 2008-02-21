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
#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_create_directory.h"
#include <iostream>
#include <omnetpp.h>
#include "client_fs_state.h"
#include "filename.h"
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
using namespace std;

FSCreateDirectory::FSCreateDirectory(FSClient* client,
                                     spfsMPIDirectoryCreateRequest* createReq)
    : client_(client),
      createReq_(createReq)
{
    assert(0 != client_);
    assert(0 != createReq_);
}

// Processing that occurs upon receipt of an MPI-IO Directory Create request
void FSCreateDirectory::handleMessage(cMessage* msg)
{
    // Restore the existing state for this Directory Create Request
    cFSM currentState = createReq_->getState();

    // File system open state machine states
    enum {
        INIT = 0,
        LOOKUP_PARENT_HANDLE = FSM_Steady(1),
        GET_PARENT_ATTRIBUTES = FSM_Steady(2),
        CREATE_META = FSM_Steady(3),
        CREATE_DATA = FSM_Steady(4),
        WRITE_ATTR = FSM_Steady(5),
        WRITE_DIRENT = FSM_Steady(6),
        FINISH = FSM_Steady(7)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIDirectoryCreateRequest*>(msg));
            bool nameCached = isParentNameCached();
            if (nameCached)
            {
                bool attrCached = isParentAttrCached();
                if (attrCached)
                    FSM_Goto(currentState, CREATE_META);
                else
                    FSM_Goto(currentState, GET_PARENT_ATTRIBUTES);
            }
            else
            {
                createReq_->setNumResolvedSegments(1);
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
            spfsLookupStatus status = processLookup(
                static_cast<spfsLookupPathResponse*>(msg));
            if (SPFS_FOUND == status)
                FSM_Goto(currentState, CREATE_META);
            else if (SPFS_PARTIAL == status)
                FSM_Goto(currentState, LOOKUP_PARENT_HANDLE);
            else
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "ERROR: Dir does not exist during creation." << endl;
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
            FSM_Goto(currentState, CREATE_META);
            break;
        }
        case FSM_Enter(CREATE_META):
        {    
            createMeta();
            break;
        }
        case FSM_Exit(CREATE_META):
        {
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            FSM_Goto(currentState, CREATE_DATA);
            break;
        }
        case FSM_Enter(CREATE_DATA):
        {
            createDataObject();
            break;
        }
        case FSM_Exit(CREATE_DATA):
        {
            FSM_Goto(currentState, WRITE_DIRENT);
            break;
        }
        case FSM_Enter(WRITE_DIRENT):
        {
            createDirEnt();
            break;
        }
        case FSM_Exit(WRITE_DIRENT):
        {
            assert(0 != dynamic_cast<spfsCreateDirEntResponse*>(msg));
            cacheNameAndAttributes();
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            finish();
            break;
        }
    }

    // Store state
    createReq_->setState(currentState);
}

bool FSCreateDirectory::isParentNameCached()
{
    // Lookup the parent directory in the name cache
    Filename createDir(createReq_->getDirName());
    Filename parentDir = createDir.getParent();

    // If the parent directory is the root, it is well known
    if (1 == parentDir.getNumPathSegments())
    {
        return true;
    }
    else
    {
        FSHandle* lookup = client_->fsState().lookupName(parentDir.str());
        return (0 != lookup);
    }
}

bool FSCreateDirectory::isParentAttrCached()
{
    // Lookup the parent directory in the name cache
    Filename createDir(createReq_->getDirName());
    Filename parentDir = createDir.getParent();
    
    FSHandle* lookup = client_->fsState().lookupName(parentDir.str());
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSCreateDirectory::lookupParentOnServer()
{
    // Set the number of resolved segments to 1 (the root directory)

    // Find the first resolved handle
    int numResolvedSegments = createReq_->getNumResolvedSegments();
    Filename dir(createReq_->getDirName());
    Filename parent = dir.getParent();
    Filename resolvedName = parent.getSegment(numResolvedSegments - 1);

    // Determine the handle of the resolved name
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(resolvedName)->handle;
    
    // Create the lookup request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(createReq_);
    req->setAutoCleanup(true);
    req->setFilename(parent.c_str());
    req->setHandle(resolvedHandle);
    req->setNumResolvedSegments(numResolvedSegments);

    // Send the request
    client_->send(req, client_->getNetOutGate());
    cerr << "Mkdir: " << dir << " looking up: " << parent << endl;
}

spfsLookupStatus FSCreateDirectory::processLookup(
    spfsLookupPathResponse* lookupResponse)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());

    // Add the lookup results
    int numResolvedSegments = lookupResponse->getNumResolvedSegments();
    createReq_->setNumResolvedSegments(numResolvedSegments);
    
    // Determine lookup results
    spfsLookupStatus lookupStatus =
        static_cast<spfsLookupStatus>(lookupResponse->getStatus());
    if (SPFS_FOUND == lookupStatus)
    {
        // Enter the parent handle into the cache
        Filename dir(createReq_->getDirName());
        Filename parent = dir.getParent();
        const FSMetaData* meta = FileBuilder::instance().getMetaData(parent);
        client_->fsState().insertName(parent.str(), meta->handle);
    }
    return lookupStatus;
}

void FSCreateDirectory::getParentAttributes()
{
    Filename dirName(createReq_->getDirName());
    Filename parent = dirName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parent);

    // Construct the request
    spfsGetAttrRequest *req = new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    req->setContextPointer(createReq_);
    req->setAutoCleanup(true);
    req->setHandle(parentMeta->handle);
    client_->send(req, client_->getNetOutGate());

    cerr << "Getting parent attrs: " << dirName << endl;
}

void FSCreateDirectory::cacheParentAttributes()
{
    Filename dirName(createReq_->getDirName());
    Filename parentName =  dirName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    client_->fsState().insertAttr(parentMeta->handle, *parentMeta);
}

void FSCreateDirectory::createMeta()
{
    // hash path to meta server number
    int metaServer = FileBuilder::instance().getMetaServers()[0];

    // Build message to create metadata
    spfsCreateRequest* req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    req->setContextPointer(createReq_);
    req->setAutoCleanup(true);
    req->setHandle(FileBuilder::instance().getFirstHandle(metaServer));
    req->setByteLength(4 + FSClient::OBJECT_ATTRIBUTES_SIZE +
                       FSClient::CREDENTIALS_SIZE + 8 + 8 + 4 + 4);
    client_->send(req, client_->getNetOutGate());
}

void FSCreateDirectory::createDataObject()
{
    // Retrieve the bookkeeping information for this directory
    Filename dirName(createReq_->getDirName());
    const FSMetaData* metaData = FileBuilder::instance().getMetaData(dirName);
    assert(0 != metaData);
    assert(1 == metaData->dataHandles.size());
    
    // Construct the create requests for this directory's data handles
    spfsCreateRequest* create = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    create->setContextPointer(createReq_);
    create->setAutoCleanup(true);
    create->setHandle(metaData->dataHandles[0]);
    create->setByteLength(4 + FSClient::OBJECT_ATTRIBUTES_SIZE +
                          FSClient::CREDENTIALS_SIZE + 8 + 8 + 4 + 4);
    client_->send(create, client_->getNetOutGate());
}

void FSCreateDirectory::createDirEnt()
{
    // Get the parent handle
    Filename dirName(createReq_->getDirName());
    Filename parentName = dirName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);

    // Construct the directory entry creation request
    spfsCreateDirEntRequest *req;
    req = new spfsCreateDirEntRequest(0, SPFS_CREATE_DIR_ENT_REQUEST);
    req->setContextPointer(createReq_);
    req->setAutoCleanup(true);
    req->setHandle(parentMeta->handle);
    req->setEntry(dirName.c_str());
    req->setByteLength(4 + dirName.str().length() + 8 + 8 + 8 + 8 +
                       FSClient::CREDENTIALS_SIZE);
    client_->send(req, client_->getNetOutGate());   
}

void FSCreateDirectory::cacheNameAndAttributes()
{
    Filename dirName(createReq_->getDirName());
    const FSMetaData* metaData = FileBuilder::instance().getMetaData(dirName);
    client_->fsState().insertName(dirName.str(), metaData->handle);
    client_->fsState().insertAttr(metaData->handle, *metaData);
}

void FSCreateDirectory::finish()
{
    spfsMPIDirectoryCreateResponse *resp = new spfsMPIDirectoryCreateResponse(
        0, SPFS_MPI_DIRECTORY_CREATE_RESPONSE);
    resp->setContextPointer(createReq_);
    client_->send(resp, client_->getAppOutGate());
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
