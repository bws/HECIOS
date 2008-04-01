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
#include "fs_delete.h"
#include <iostream>
#include <omnetpp.h>
#include "client_fs_state.h"
#include "filename.h"
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSDelete::FSDelete(FSClient* client,
                   spfsMPIFileDeleteRequest* deleteReq,
                   bool useCollectiveCommunication)
    : client_(client),
      clientReq_(deleteReq),
      useCollectiveCommunication_(useCollectiveCommunication),
      deleteName_(deleteReq->getFileName())
{
    assert(0 != client_);
    assert(0 != clientReq_);
}

FSDelete::FSDelete(FSClient* client,
                   spfsMPIDirectoryRemoveRequest* rmDirReq,
                   bool useCollectiveCommunication)
    : client_(client),
      clientReq_(rmDirReq),
      useCollectiveCommunication_(useCollectiveCommunication),
      deleteName_(rmDirReq->getDirName())
{
    assert(0 != client_);
    assert(0 != clientReq_);
}

void FSDelete::handleMessage(cMessage* msg)
{
    if (useCollectiveCommunication_)
    {
        collectiveMessageProcessor(msg);
    }
    else
    {
        serialMessageProcessor(msg);
    }
}

void FSDelete::serialMessageProcessor(cMessage* msg)
{
    /** Restore the existing state for this Open Request */
    cFSM currentState = clientReq_->getState();

    /** File system open state machine states */
    enum {
        INIT = 0,
        LOOKUP_PARENT_HANDLE = FSM_Steady(1),
        GET_PARENT_ATTRIBUTES = FSM_Steady(2),
        REMOVE_DIRENT = FSM_Steady(5),
        REMOVE_META = FSM_Steady(6),
        REMOVE_DATA = FSM_Transient(7),
        COUNT_DATA_RESPONSES = FSM_Steady(8),
        FINISH = FSM_Steady(9)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            bool nameCached = isParentNameCached();
            if (nameCached)
            {
                bool attrCached = isParentAttrCached();
                if (attrCached)
                {
                    FSM_Goto(currentState, REMOVE_DIRENT);
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
            FSM_Goto(currentState, REMOVE_DIRENT);
            break;
        }
        case FSM_Enter(REMOVE_DIRENT):
        {
            removeDirEnt();
            break;
        }
        case FSM_Exit(REMOVE_DIRENT):
        {
            FSM_Goto(currentState, REMOVE_META);
            break;
        }
        case FSM_Enter(REMOVE_META):
        {
            removeMeta();
            break;
        }
        case FSM_Exit(REMOVE_META):
        {
            FSM_Goto(currentState, REMOVE_DATA);
            break;
        }
        case FSM_Enter(REMOVE_DATA):
        {    
            removeData();
            break;
        }
        case FSM_Exit(REMOVE_DATA):
        {
            FSM_Goto(currentState, COUNT_DATA_RESPONSES);
            break;
        }
        case FSM_Exit(COUNT_DATA_RESPONSES):
        {
            assert(0 != dynamic_cast<spfsRemoveResponse*>(msg));
            bool isDataRemovalComplete = countDataRemoveResponse();
            if (isDataRemovalComplete)
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT_DATA_RESPONSES);
            }
            break;
        }
        case FSM_Enter(FINISH):
        {
            finish();
            break;
        }
    }

    // Store state
    clientReq_->setState(currentState);
}

void FSDelete::collectiveMessageProcessor(cMessage* msg)
{
    /** Restore the existing state for this Open Request */
    cFSM currentState = clientReq_->getState();

    /** File system open state machine states */
    enum {
        INIT = 0,
        LOOKUP_PARENT_HANDLE = FSM_Steady(1),
        GET_PARENT_ATTRIBUTES = FSM_Steady(2),
        COLLECTIVE_REMOVE = FSM_Steady(5),
        FINISH = FSM_Steady(10)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            bool nameCached = isParentNameCached();
            if (nameCached)
            {
                bool attrCached = isParentAttrCached();
                if (attrCached)
                {
                    FSM_Goto(currentState, COLLECTIVE_REMOVE);
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
            FSM_Goto(currentState, COLLECTIVE_REMOVE);
            break;
        }
        case FSM_Enter(COLLECTIVE_REMOVE):
        {    
            collectiveRemove();
            break;
        }
        case FSM_Exit(COLLECTIVE_REMOVE):
        {
            assert(0 != dynamic_cast<spfsCollectiveRemoveResponse*>(msg));
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
    clientReq_->setState(currentState);
}

bool FSDelete::isParentNameCached()
{
    // Lookup the parent directory in the name cache
    Filename parentDir = deleteName_.getParent();

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

bool FSDelete::isParentAttrCached()
{
    // Lookup the parent directory in the name cache
    Filename parentDir = deleteName_.getParent();
    
    FSHandle* lookup = client_->fsState().lookupName(parentDir.str());
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSDelete::lookupParentOnServer()
{
    // Find the first resolved handle
    int numResolvedSegments = clientReq_->getNumResolvedSegments();
    Filename parent = deleteName_.getParent();
    Filename resolvedName = parent.getSegment(numResolvedSegments - 1);

    // Determine the handle of the resolved name
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(resolvedName)->handle;
    
    spfsLookupPathRequest* req = FSClient::createLookupPathRequest(
        parent, resolvedHandle, numResolvedSegments);
    req->setContextPointer(clientReq_);
    
    // Send the request
    client_->send(req, client_->getNetOutGate());
}

FSLookupStatus FSDelete::processLookup(spfsLookupPathResponse* lookupResponse)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());

    // Add the lookup results
    int numResolvedSegments = lookupResponse->getNumResolvedSegments();
    clientReq_->setNumResolvedSegments(numResolvedSegments);
    
    // Determine lookup results
    FSLookupStatus lookupStatus = lookupResponse->getStatus();
    if (SPFS_FOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename resolvedName = deleteName_.getSegment(numResolvedSegments - 1);
        const FSMetaData* meta =
            FileBuilder::instance().getMetaData(resolvedName);
        client_->fsState().insertName(resolvedName.str(), meta->handle);
    }
    else if (SPFS_NOTFOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename resolvedName = deleteName_.getSegment(numResolvedSegments - 1);
        cerr << "Could not open: " << deleteName_ << " unable to resolve past: "
             << resolvedName << endl;
    }
    return lookupStatus;
}

void FSDelete::getParentAttributes()
{
    Filename parent = deleteName_.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parent);

    // Construct the request
    cerr << "Getting parent attributes: " << parent << endl;
    spfsGetAttrRequest *req =
        FSClient::createGetAttrRequest(parentMeta->handle,
                                       SPFS_METADATA_OBJECT);
    req->setContextPointer(clientReq_);
    req->setAutoCleanup(true);
    client_->send(req, client_->getNetOutGate());
}

void FSDelete::cacheParentAttributes()
{
    Filename parentName =  deleteName_.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    client_->fsState().insertAttr(parentMeta->handle, *parentMeta);
}

void FSDelete::removeDirEnt()
{
    Filename parentName =  deleteName_.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);

    spfsRemoveDirEntRequest* removeDirEnt =
        FSClient::createRemoveDirEntRequest(parentMeta->handle, deleteName_);
    removeDirEnt->setContextPointer(clientReq_);
    client_->send(removeDirEnt, client_->getNetOutGate());
}

void FSDelete::removeMeta()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(deleteName_);

    spfsRemoveRequest* remove =
        FSClient::createRemoveRequest(meta->handle, SPFS_METADATA_OBJECT);
    remove->setContextPointer(clientReq_);
    client_->send(remove, client_->getNetOutGate());
}

void FSDelete::removeData()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(deleteName_);

    // Schedule the remove messages
    for (size_t i = 0; i < meta->dataHandles.size(); i++)
    {
        spfsRemoveRequest* remove =
            FSClient::createRemoveRequest(meta->dataHandles[i],
                                          SPFS_DATA_OBJECT);
        remove->setContextPointer(clientReq_);
        client_->send(remove, client_->getNetOutGate());
    }

    // Initialize the response counter
    clientReq_->setRemainingResponses(meta->dataHandles.size());
}

bool FSDelete::countDataRemoveResponse()
{
    // Decrement the number of create responses to expect
    int numResponses = clientReq_->getRemainingResponses();
    clientReq_->setRemainingResponses(--numResponses);

    bool isComplete = false;
    if (0 == numResponses)
    {
        isComplete = true;
    }
    return isComplete;
}

void FSDelete::finish()
{
    cMessage* finalResponse = 0;
    if (SPFS_MPI_FILE_DELETE_REQUEST == clientReq_->kind())
    {
        finalResponse = new spfsMPIFileDeleteResponse(
            0, SPFS_MPI_FILE_DELETE_RESPONSE);
    }
    else
    {
        assert(SPFS_MPI_DIRECTORY_REMOVE_REQUEST == clientReq_->kind());
        finalResponse = new spfsMPIDirectoryRemoveResponse(
            0, SPFS_MPI_DIRECTORY_REMOVE_RESPONSE);
    }
    
    finalResponse->setContextPointer(clientReq_);
    client_->send(finalResponse, client_->getAppOutGate());
}

void FSDelete::collectiveRemove()
{
    // Get the metadata handle
    Filename parentName = deleteName_.getParent();
    FSMetaData* meta = FileBuilder::instance().getMetaData(deleteName_);
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    assert(0 != meta);
    assert(0 != parentMeta);
    
    spfsCollectiveRemoveRequest* req = FSClient::createCollectiveRemoveRequest(
        parentMeta->dataHandles[0], meta->handle, meta->dataHandles);
    req->setContextPointer(clientReq_);
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
