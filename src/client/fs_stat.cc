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
#include "pvfs_proto_m.h"
using namespace std;

FSStat::FSStat(FSClient* client,
               spfsMPIFileStatRequest* statReq,
               bool useCollectiveCommunication)
    : client_(client),
      statReq_(statReq),
      useCollectiveCommunication_(useCollectiveCommunication)
{
    assert(0 != client_);
    assert(0 != statReq_);
}

void FSStat::handleMessage(cMessage* msg)
{
    if (useCollectiveCommunication_ &&
        true == statReq_->getDetermineFileSize())
    {
        collectiveMessageProcessor(msg);
    }
    else
    {
        serialMessageProcessor(msg);
    }
}

void FSStat::serialMessageProcessor(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = statReq_->getState();

    // File system stat state machine states
    enum {
        INIT = 0,
        LOOKUP_PARENT_HANDLE = FSM_Steady(1),
        GET_PARENT_ATTRIBUTES = FSM_Steady(2),
        LOOKUP_NAME = FSM_Steady(4),
        GET_METADATA_ATTR = FSM_Steady(5),
        GET_DATA_ATTRS = FSM_Transient(6),
        COUNT_RESPONSES = FSM_Steady(7),
        FINISH = FSM_Steady(8)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileStatRequest*>(msg));
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
            FSM_Goto(currentState, GET_METADATA_ATTR);
            break;
        }
        case FSM_Enter(GET_METADATA_ATTR):
        {
            getMetaDataAttributes();
            break;
        }
        case FSM_Exit(GET_METADATA_ATTR):
        {
            assert(0 != dynamic_cast<spfsGetAttrResponse*>(msg));
            FSM_Goto(currentState, GET_DATA_ATTRS);
            break;
        }
        case FSM_Enter(GET_DATA_ATTRS):
        {
            getDataAttributes();
            break;
        }
        case FSM_Exit(GET_DATA_ATTRS):
        {
            FSM_Goto(currentState, COUNT_RESPONSES);
            break;
        }        
        case FSM_Enter(COUNT_RESPONSES):
        {
            break;
        }
        case FSM_Exit(COUNT_RESPONSES):
        {
            bool isFinished = processResponse(msg);
            if (isFinished)
                FSM_Goto(currentState, FINISH);
            else
                FSM_Goto(currentState, COUNT_RESPONSES);
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

void FSStat::collectiveMessageProcessor(cMessage* msg)
{
    /** Restore the existing state for this Open Request */
    cFSM currentState = statReq_->getState();

    /** File system open state machine states */
    enum {
        INIT = 0,
        LOOKUP_PARENT_HANDLE = FSM_Steady(1),
        GET_PARENT_ATTRIBUTES = FSM_Steady(2),
        COLLECTIVE_STAT = FSM_Steady(5),
        FINISH = FSM_Steady(10)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileStatRequest*>(msg));
            bool nameCached = isParentNameCached();
            if (nameCached)
            {
                bool attrCached = isParentAttrCached();
                if (attrCached)
                {
                    FSM_Goto(currentState, COLLECTIVE_STAT);
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
            FSM_Goto(currentState, COLLECTIVE_STAT);
            break;
        }
        case FSM_Enter(COLLECTIVE_STAT):
        {    
            collectiveStat();
            break;
        }
        case FSM_Exit(COLLECTIVE_STAT):
        {
            assert(0 != dynamic_cast<spfsCollectiveGetAttrResponse*>(msg));
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

void FSStat::getMetaDataAttributes()
{
    Filename statName(statReq_->getFileName());
    const FSMetaData* meta = FileBuilder::instance().getMetaData(statName);
    spfsGetAttrRequest* req = FSClient::createGetAttrRequest(
        meta->handle, SPFS_METADATA_OBJECT);
    req->setContextPointer(statReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSStat::getDataAttributes()
{
    if (statReq_->getDetermineFileSize())
    {
        Filename statName(statReq_->getFileName());
        const FSMetaData* meta = FileBuilder::instance().getMetaData(statName);

        for (size_t i = 0; i < meta->dataHandles.size(); i++)
        {
            spfsGetAttrRequest* req = FSClient::createGetAttrRequest(
                meta->dataHandles[i], SPFS_DATA_OBJECT);
            req->setContextPointer(statReq_);
            client_->send(req, client_->getNetOutGate());
        }

        statReq_->setRemainingResponses(meta->dataHandles.size());
    }
    else
    {
        statReq_->setRemainingResponses(0);
    }
}

bool FSStat::processResponse(cMessage* getAttrResponse)
{
    assert(0 != dynamic_cast<spfsGetAttrResponse*>(getAttrResponse));

    bool isComplete = false;
    int numOutstanding = statReq_->getRemainingResponses() - 1;
    statReq_->setRemainingResponses(numOutstanding);
    if (0 == numOutstanding)
    {
        isComplete = true;
    }
    return isComplete;
}

void FSStat::collectiveStat()
{
    // Get the parent handle
    Filename statName(statReq_->getFileName());
    FSMetaData* meta = FileBuilder::instance().getMetaData(statName);

    spfsCollectiveGetAttrRequest* req =
        FSClient::createCollectiveGetAttrRequest(meta->handle,
                                                 meta->dataHandles);
    req->setContextPointer(statReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSStat::finish()
{
    spfsMPIFileStatResponse* mpiResp =
        new spfsMPIFileStatResponse(0, SPFS_MPI_FILE_STAT_RESPONSE);
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
