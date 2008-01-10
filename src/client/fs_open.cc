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
        LOOKUP = FSM_Steady(1),
        CREATE_META = FSM_Steady(2),
        CREATE_DATA = FSM_Transient(3),
        COUNT_DATA_RESPONSES = FSM_Steady(4),
        WRITE_ATTR = FSM_Steady(5),
        WRITE_DIRENT = FSM_Steady(6),
        READ_ATTR = FSM_Steady(7),
        FINISH = FSM_Steady(8)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileOpenRequest*>(msg));
            bool isInDirCache, isInAttrCache;
            exitInit(static_cast<spfsMPIFileOpenRequest*>(msg),
                     isInDirCache, isInAttrCache);
            if (isInDirCache && isInAttrCache)
                FSM_Goto(currentState, FINISH);
            else if (isInDirCache)
                FSM_Goto(currentState, READ_ATTR);
            else
                FSM_Goto(currentState, LOOKUP);
            break;
        }
        case FSM_Enter(LOOKUP):
        {
            lookupOnServer();
            break;
        }
        case FSM_Exit(LOOKUP):
        {
            assert(0 != dynamic_cast<spfsLookupPathResponse*>(msg));
            bool isCreate, isFullLookup, isMissingAttr, isPartialLookup;
            exitLookup(static_cast<spfsLookupPathResponse*>(msg),
                       isCreate, isFullLookup, isMissingAttr, isPartialLookup);
            if (isCreate)
                FSM_Goto(currentState, CREATE_META);
            else if (isFullLookup)
                FSM_Goto(currentState, FINISH);
            else if (isMissingAttr)
                FSM_Goto(currentState, READ_ATTR);
            else if (isPartialLookup)
                FSM_Goto(currentState, LOOKUP);
            else
            {
                cerr << "ERROR: File does not exist during open!!!!" << endl;
            }

            // Remove the lookup request
            delete (cMessage*)msg->contextPointer();
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
        case FSM_Enter(READ_ATTR):
        {    
            readAttributes();
            break;
        }
        case FSM_Exit(READ_ATTR):
        {
            assert(0 != dynamic_cast<spfsGetAttrResponse*>(msg));
            addAttributesToCache();
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

void FSOpen::exitInit(spfsMPIFileOpenRequest* openReq,
                      bool& outIsInDirCache,
                      bool& outIsInAttrCache)
{
    // Preconditions
    assert(0 != openReq->getFileName());
    assert(0 != openReq->getFileDes());
    assert(0 == (openReq->getMode() & MPI_MODE_EXCL));
    
    // Initialize outbound variable
    outIsInDirCache = false;
    outIsInAttrCache = false;

    // Lookup the file name in the cache
    Filename openFile(openReq->getFileName());
    FSHandle* lookup = client_->fsState().lookupName(openFile.str());
    if (0 != lookup)
    {
        // Found directory cache entry
        outIsInDirCache = true;
        
        // Determine if the metadata is cached
        FSMetaData* meta = client_->fsState().lookupAttr(*lookup);
        if (0 != meta)
        {
            // Found attribute cache entry
            outIsInAttrCache = true;            
        }
    }
}

void FSOpen::lookupOnServer()
{
    // retrieve the file descriptor
    FileDescriptor* fd =  openReq_->getFileDes();

    // Get the closest resolved parent handle
    int numParentHandles = fd->getNumParentHandles();
    FSHandle parentHandle = fd->getParentHandle(numParentHandles - 1);

    // Create the lookup request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(openReq_);
    
    // Set the parent directory handle
    req->setFilename(fd->getFilename().c_str());
    req->setHandle(parentHandle);
    req->setNumResolvedSegments(numParentHandles);

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::exitLookup(spfsLookupPathResponse* lookupResponse,
                        bool& outIsCreate, bool& outIsFullLookup,
                        bool& outIsMissingAttr, bool& outIsPartialLookup)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());
    
    // Initialize outbound data
    outIsCreate = false;
    outIsFullLookup = false;
    outIsMissingAttr = false;
    outIsPartialLookup = false;

    // Determine lookup results
    switch (lookupResponse->getStatus())
    {
        case SPFS_FOUND:
        {
            /* enter handle in cache */
            FileDescriptor* filedes = openReq_->getFileDes();
            const FSMetaData* meta = filedes->getMetaData();
            client_->fsState().insertName(filedes->getFilename().str(),
                                          meta->handle);
            
            /* look for metadata in cache */
            if (0 == client_->fsState().lookupAttr(meta->handle))
            {
                outIsMissingAttr = true;
            }
            else
            {
                outIsFullLookup = true;
            }
            break;
        }
        case SPFS_PARTIAL:
        {
            outIsPartialLookup = true;
            break;
        }
        case SPFS_NOTFOUND:
        {
            assert(openReq_->getMode() & MPI_MODE_CREATE);
            outIsCreate = true;
            break;
        }
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
