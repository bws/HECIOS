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
        CREATE_DATA = FSM_Steady(3),
        WRITE_ATTR = FSM_Steady(4),
        WRITE_DIRENT = FSM_Steady(5),
        READ_ATTR = FSM_Steady(6),
        FINISH = FSM_Steady(7),
        ERRNF = FSM_Steady(8),
        ERREXCL = FSM_Steady(9),        
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
            enterLookup();
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
            break;
        }
        case FSM_Enter(CREATE_META):
        {    
            enterCreateMeta();
            break;
        }
        case FSM_Exit(CREATE_META):
        {
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            exitCreateMeta(static_cast<spfsCreateResponse*>(msg));
            FSM_Goto(currentState, CREATE_DATA);
            break;
        }
        case FSM_Enter(CREATE_DATA):
        {
            enterCreateData();
            break;
        }
        case FSM_Exit(CREATE_DATA):
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            bool isDoneCreatingDataObjects;
            exitCreateData(static_cast<spfsCreateResponse*>(msg),
                           isDoneCreatingDataObjects);
            if (isDoneCreatingDataObjects)
                FSM_Goto(currentState, WRITE_ATTR);
            break;
        case FSM_Enter(WRITE_ATTR):
        {
            enterWriteAttr();
            break;
        }
        case FSM_Exit(WRITE_ATTR):
        {
            assert(0 != dynamic_cast<spfsSetAttrResponse*>(msg));
            exitWriteAttr(static_cast<spfsSetAttrResponse*>(msg));
            FSM_Goto(currentState, WRITE_DIRENT);
            break;
        }
        case FSM_Enter(WRITE_DIRENT):
        {
            enterWriteDirEnt();
            break;
        }
        case FSM_Exit(WRITE_DIRENT):
        {
            assert(0 != dynamic_cast<spfsCreateDirEntResponse*>(msg));
            exitWriteDirEnt(static_cast<spfsCreateDirEntResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(READ_ATTR):
        {    
            enterReadAttr();
            break;
        }
        case FSM_Exit(READ_ATTR):
        {
            assert(0 != dynamic_cast<spfsGetAttrResponse*>(msg));
            exitReadAttr(static_cast<spfsGetAttrResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            enterFinish();
            break;
        }
        default:
            cerr << "Error: Illegal open state entered: " << currentState
                 << endl;
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

    // Retrieve the data from the simulator bookkeeper
    Filename openFile(openReq->getFileName());
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq->getFileDes());
    fd->metaData = FileBuilder::instance().getMetaData(openFile);
    fd->path = openFile.str();
    
    /* look for dir in cache */
    FSHandle* lookup = client_->fsState().lookupDir(fd->path);
    if (0 == lookup)
    {
        /* dir entry not in cache go do lookup */
        PFSUtils::instance().parsePath(fd);
        //FIXME fd->handles[0] = fsModule_->fsState().root();
        fd->curseg = 0;
    }
    else
    {
        // Found directory cache entry
        outIsInDirCache = true;
        
        /* dir entry in cache look for metadata in cache */
        FSMetaData* meta = client_->fsState().lookupAttr(*lookup);
        if (0 != meta)
        {
            // Found attribute cache entry
            outIsInAttrCache = true;            
        }
    }
}

void FSOpen::enterLookup()
{
    // Create the new request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(openReq_);
    
    /* copy path to look up */
    FSOpenFile *filedes = (FSOpenFile *)openReq_->getFileDes();
    req->setPath(
        filedes->path.substr(filedes->segstart[filedes->curseg],
                             filedes->seglen[filedes->curseg]).c_str());
    
    /* get handle for root dir */
    Filename root("/");
    FSMetaData* rootMeta = FileBuilder::instance().getMetaData(root);
    req->setHandle(rootMeta->handle);
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::exitLookup(spfsLookupPathResponse* lookupResponse,
                        bool& outIsCreate, bool& outIsFullLookup,
                        bool& outIsMissingAttr, bool& outIsPartialLookup)
{
    // Preconditions
    assert(0 < lookupResponse->getHandleCount());
    
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
            FSOpenFile* filedes = (FSOpenFile*)openReq_->getFileDes();
            client_->fsState().insertDir(filedes->path,
                                           filedes->metaData->handle);
            /* look for metadata in cache */
            FSMetaData* meta =
                client_->fsState().lookupAttr(filedes->metaData->handle);
            if (0 == meta)
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

    delete lookupResponse;
}

void FSOpen::enterCreateMeta()
{
    /* build a request message */
    spfsCreateRequest* req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    req->setContextPointer(openReq_);

    // hash path to meta server number
    int metaServer = FileBuilder::instance().getMetaServers()[0];

    req->setHandle(FileBuilder::instance().getFirstHandle(metaServer));
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::exitCreateMeta(spfsCreateResponse* createResp)
{
    // Don't need to do anything, data already exists in simulation
}

void FSOpen::enterCreateData()
{
    spfsCreateRequest *req;
    int numservers;
    int snum;
    /* build a request message */
    req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    req->setContextPointer(openReq_);
    numservers = client_->fsState().defaultNumServers();
    /* send request to each server */
    for (snum = 0; snum < numservers; snum++)
    {
        spfsCreateRequest* newReq = (spfsCreateRequest*)req->dup();

        // Set the message size in bytes
        //newReq->setByteLength(8);
            
        // use first handle of range to address server
        int dataServer = client_->fsState().selectServer();
        newReq->setHandle(FileBuilder::instance().getFirstHandle(dataServer));
        client_->send(newReq, client_->getNetOutGate());
    }

    delete req;
    /* indicates how many responses we are waiting for */
    openReq_->setResponses(numservers);
}

void FSOpen::exitCreateData(spfsCreateResponse* createResp,
                            bool& outIsDoneCreatingDataObjects)
{
    // Initialize outbound data
    outIsDoneCreatingDataObjects = false;
}

void FSOpen::enterWriteAttr()
{
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFileDes());
    spfsSetAttrRequest *req = new spfsSetAttrRequest(0, SPFS_SET_ATTR_REQUEST);
    req->setContextPointer(openReq_);
    req->setHandle(fd->metaData->handle);
    req->setMeta(*fd->metaData);
    client_->send(req, client_->getNetOutGate());   
}

void FSOpen::exitWriteAttr(spfsSetAttrResponse* sattrResp)
{
}

void FSOpen::enterWriteDirEnt()
{
    spfsCreateDirEntRequest *req;
    req = new spfsCreateDirEntRequest(0, SPFS_CREATE_DIR_ENT_REQUEST);
    req->setContextPointer(openReq_);
    /* this addresses the server with the dir on it */
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFileDes());
    req->setHandle(fd->handles[fd->curseg]);
    /* this is the same handle, the handle of the parent dir */
    /* can we get rid of this field?  WBL */
    req->setParentHandle(fd->handles[fd->curseg]);
    /* this is the handle of the new file goes in dirent */
    req->setNewHandle(fd->metaData->handle);
    /* this is the name of the file */
    req->setEntry(openReq_->getFileName());
    client_->send(req, client_->getNetOutGate());   
}

void FSOpen::exitWriteDirEnt(spfsCreateDirEntResponse* dirEntResp)
{
    // FIXME do anything ?? */
    /* free create dirent response message */
    delete dirEntResp;

}

void FSOpen::enterReadAttr()
{
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFileDes());
    spfsGetAttrRequest *req = new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    req->setContextPointer(openReq_);
    req->setHandle(fd->metaData->handle);
    client_->send(req, client_->getNetOutGate());
}

void FSOpen::exitReadAttr(spfsGetAttrResponse* gattrResp)
{
    // FIXME install attributes into attribute cache

    /* free get attr resp message */
    delete gattrResp;
}

void FSOpen::enterFinish()
{
    spfsMPIFileOpenResponse *resp = new spfsMPIFileOpenResponse(
        0, SPFS_MPI_FILE_OPEN_RESPONSE);
    resp->setContextPointer(openReq_);
    resp->setFileDes(openReq_->getFileDes());
    client_->send(resp, client_->getAppOutGate());
}
            
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
