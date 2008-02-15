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
#include "fs_open.h"
#include <iostream>
//#define FSM_DEBUG  // Enable FSM Debug output
#include <omnetpp.h>
#include "client_fs_state.h"
#include "filename.h"
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSOpen::FSOpen(FSClient* client, spfsMPIFileOpenRequest* openReq)
    : client_(client),
      openReq_(openReq)
{
    assert(0 != client_);
    assert(0 != openReq_);
}

// Processing that occurs upon receipt of an MPI-IO Open request
void FSOpen::handleMessage(cMessage* msg)
{
    /** Restore the existing state for this Open Request */
    cFSM currentState = openReq_->getState();

    /** File system open state machine states */
    enum {
        INIT = 0,
        LOOKUP_PARENT_HANDLE = FSM_Steady(1),
        GET_PARENT_ATTRIBUTES = FSM_Steady(2),
        LOOKUP_NAME = FSM_Steady(4),
        CREATE_META = FSM_Steady(5),
        CREATE_DATA = FSM_Transient(6),
        COUNT_DATA_RESPONSES = FSM_Steady(7),
        WRITE_ATTR = FSM_Steady(8),
        WRITE_DIRENT = FSM_Steady(9),
        FINISH = FSM_Steady(10)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileOpenRequest*>(msg));
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
            spfsLookupStatus status = processLookup(
                static_cast<spfsLookupPathResponse*>(msg));
            if (SPFS_FOUND == status)
                FSM_Goto(currentState, GET_PARENT_ATTRIBUTES);
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
            spfsLookupStatus status = processLookup(
                static_cast<spfsLookupPathResponse*>(msg));
            bool isCreate = checkFileCreateFlags(status);
            if (isCreate)
                FSM_Goto(currentState, CREATE_META);
            else
                FSM_Goto(currentState, FINISH); 
            break;
        }
        case FSM_Enter(CREATE_META):
        {    
            assert(0 != dynamic_cast<spfsLookupPathResponse*>(msg));
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
            createDataObjects();
            break;
        }
        case FSM_Exit(CREATE_DATA):
        {
            FSM_Goto(currentState, COUNT_DATA_RESPONSES);
            break;
        }
        case FSM_Exit(COUNT_DATA_RESPONSES):
        {
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            countDataCreationResponse();
            if (isDataCreationComplete())
            {
                FSM_Goto(currentState, WRITE_ATTR);
            }
            else
            {
                FSM_Goto(currentState, COUNT_DATA_RESPONSES);
            }
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
    openReq_->setState(currentState);
}

bool FSOpen::isParentNameCached()
{
    // Lookup the parent directory in the name cache
    Filename openFile(openReq_->getFileName());
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

bool FSOpen::isParentAttrCached()
{
    // Lookup the parent directory in the name cache
    Filename openFile(openReq_->getFileName());
    Filename parentDir = openFile.getParent();
    
    FSHandle* lookup = client_->fsState().lookupName(parentDir.str());
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSOpen::lookupParentOnServer()
{
    // Find the first resolved handle
    int numResolvedSegments = openReq_->getNumResolvedSegments();
    Filename openFile(openReq_->getFileName());
    Filename parent = openFile.getParent();
    Filename resolvedName = parent.getSegment(numResolvedSegments - 1);

    // Determine the handle of the resolved name
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(resolvedName)->handle;
    
    // Create the lookup request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(openReq_);
    req->setAutoCleanup(true);
    req->setFilename(parent.c_str());
    req->setHandle(resolvedHandle);
    req->setNumResolvedSegments(numResolvedSegments);

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::getParentAttributes()
{
    Filename openName(openReq_->getFileName());
    Filename parent = openName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parent);

    // Construct the request
    spfsGetAttrRequest *req = new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    req->setContextPointer(openReq_);
    req->setAutoCleanup(true);
    req->setHandle(parentMeta->handle);
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::cacheParentAttributes()
{
    Filename openName(openReq_->getFileName());
    Filename parentName =  openName.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    client_->fsState().insertAttr(parentMeta->handle, *parentMeta);
}

void FSOpen::lookupNameOnServer()
{
    // Find the first resolved handle
    Filename openFile(openReq_->getFileName());
    Filename parent = openFile.getParent();

    // Determine the handle of the parent
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(parent)->handle;
    
    // Create the lookup request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(openReq_);
    req->setAutoCleanup(true);
    req->setFilename(openFile.c_str());
    req->setHandle(resolvedHandle);
    req->setNumResolvedSegments(parent.getNumPathSegments());

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

spfsLookupStatus FSOpen::processLookup(spfsLookupPathResponse* lookupResponse)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());

    // Add the lookup results
    int numResolvedSegments = lookupResponse->getNumResolvedSegments();
    openReq_->setNumResolvedSegments(numResolvedSegments);
    
    // Determine lookup results
    spfsLookupStatus lookupStatus =
        static_cast<spfsLookupStatus>(lookupResponse->getStatus());
    if (SPFS_FOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename openFile(openReq_->getFileName());
        Filename resolvedName = openFile.getSegment(numResolvedSegments - 1);
        const FSMetaData* meta =
            FileBuilder::instance().getMetaData(resolvedName);
        cerr << "Adding name to cache: " << resolvedName << endl;
        client_->fsState().insertName(resolvedName.str(), meta->handle);
    }
    return lookupStatus;
}

bool FSOpen::checkFileCreateFlags(const spfsLookupStatus& status)
{
    // TODO
    //assert((status != SPFS_FOUND) && (openReq_->getMode() & MPI_MODE_CREATE));
    if (openReq_->getMode() & MPI_MODE_CREATE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void FSOpen::createMeta()
{
    // Build message to create metadata
    spfsCreateRequest* req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    req->setContextPointer(openReq_);

    // hash path to meta server number
    int metaServer = FileBuilder::instance().getMetaServers()[0];

    req->setHandle(FileBuilder::instance().getFirstHandle(metaServer));
    req->setByteLength(4 + FSClient::OBJECT_ATTRIBUTES_SIZE +
                       FSClient::CREDENTIALS_SIZE + 8 + 8 + 4 + 4);
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::createDataObjects()
{
    // Retrieve the bookkeeping information for this file
    FileDescriptor* fd = openReq_->getFileDes();
    const FSMetaData* metaData = fd->getMetaData();

    // Construct the create requests for this file's data handles
    for (size_t i = 0; i < metaData->dataHandles.size(); i++)
    {
        spfsCreateRequest* create = new spfsCreateRequest(0,
                                                          SPFS_CREATE_REQUEST);
        create->setContextPointer(openReq_);
        create->setHandle(metaData->dataHandles[i]);
        create->setByteLength(8);
        client_->send(create, client_->getNetOutGate());
    }

    // Set the number of create responses to expect
    openReq_->setRemainingResponses(metaData->dataHandles.size());
}

void FSOpen::countDataCreationResponse()
{
    // Decrement the number of create responses to expect
    int numResponses = openReq_->getRemainingResponses();
    openReq_->setRemainingResponses(--numResponses);
}

bool FSOpen::isDataCreationComplete()
{
    int numResponses = openReq_->getRemainingResponses();
    if (0 == numResponses)
        return true;
    return false;
}

void FSOpen::writeAttributes()
{
    FileDescriptor* fd = openReq_->getFileDes();
    spfsSetAttrRequest *req = new spfsSetAttrRequest(0, SPFS_SET_ATTR_REQUEST);
    req->setContextPointer(openReq_);
    req->setHandle(fd->getMetaData()->handle);
    req->setByteLength(8 + 8 + 64);
    client_->send(req, client_->getNetOutGate());

    // Add the attributes to the cache
    addAttributesToCache();
}

void FSOpen::createDirEnt()
{
    // Get the parent handle
    FileDescriptor* fd = openReq_->getFileDes();
    size_t numPathSegments = fd->getFilename().getNumPathSegments();
    Filename parentName = fd->getFilename().getSegment(numPathSegments - 2);
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);

    // Construct the directory entry creation request
    spfsCreateDirEntRequest *req;
    req = new spfsCreateDirEntRequest(0, SPFS_CREATE_DIR_ENT_REQUEST);
    req->setContextPointer(openReq_);
    req->setHandle(parentMeta->handle);
    req->setEntry(fd->getFilename().c_str());
    req->setByteLength(8 + 8 + fd->getFilename().str().length());
    client_->send(req, client_->getNetOutGate());   
}

void FSOpen::readAttributes()
{
    FileDescriptor* fd = openReq_->getFileDes();
    spfsGetAttrRequest *req = new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    req->setContextPointer(openReq_);
    req->setHandle(fd->getMetaData()->handle);
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::addAttributesToCache()
{
    FileDescriptor* fd = openReq_->getFileDes();
    const FSMetaData* metaData = fd->getMetaData();
    client_->fsState().insertAttr(metaData->handle, *metaData);
}

void FSOpen::finish()
{
    spfsMPIFileOpenResponse *resp = new spfsMPIFileOpenResponse(
        0, SPFS_MPI_FILE_OPEN_RESPONSE);
    resp->setContextPointer(openReq_);
    resp->setFileDes(openReq_->getFileDes());
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
