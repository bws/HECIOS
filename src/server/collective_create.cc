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
#include <cassert>
#include <omnetpp.h>
#include "collective_create.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

CollectiveCreate::CollectiveCreate(FSServer* module,
                                   spfsCollectiveCreateRequest* createReq)
    : module_(module),
      createReq_(createReq)
{
}

void CollectiveCreate::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = createReq_->getState();

    // Server create states
    enum {
        INIT = 0,
        SEND_META = FSM_Steady(1),
        CREATE_META = FSM_Steady(2),
        CREATE_DFILE = FSM_Transient(3),
        SEND_REQUESTS = FSM_Transient(4),
        WAIT_FOR_RESPONSE = FSM_Steady(5),
        SET_ATTR = FSM_Steady(6),
        CREATE_DIR_ENT = FSM_Steady(7),
        FINISH = FSM_Steady(8),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsCollectiveCreateRequest*>(msg));
            module_->recordCollectiveCreate();
            createReq_->setNumOutstandingRequests(0);
            FSObjectType objectType = createReq_->getObjectType();
            if (SPFS_DIR_ENT_OBJECT == objectType)
            {
                FSM_Goto(currentState, SEND_META);
            }
            else if (SPFS_METADATA_OBJECT == objectType)
            {
                FSM_Goto(currentState, CREATE_META);
            }
            else
            {
                assert(SPFS_DATA_OBJECT == objectType);
                FSM_Goto(currentState, CREATE_DFILE);
            }
            break;
        }
        case FSM_Enter(SEND_META):
        {
            assert(0 != dynamic_cast<spfsCollectiveCreateRequest*>(msg));
            sendMeta();
            break;
        }
        case FSM_Exit(SEND_META):
        {
            FSM_Goto(currentState, SEND_REQUESTS);
            break;
        }
        case FSM_Enter(CREATE_META):
        {
            assert(0 != dynamic_cast<spfsCollectiveCreateRequest*>(msg));
            enterCreate();
            break;
        }
        case FSM_Exit(CREATE_META):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(CREATE_DFILE):
        {
            assert(0 != dynamic_cast<spfsCollectiveCreateRequest*>(msg));
            enterCreate();
            break;
        }
        case FSM_Exit(CREATE_DFILE):
        {
            FSM_Goto(currentState, SEND_REQUESTS);
            break;
        }
        case FSM_Enter(SEND_REQUESTS):
        {
            sendCollectiveRequests();
            break;
        }
        case FSM_Exit(SEND_REQUESTS):
        {
            FSM_Goto(currentState, WAIT_FOR_RESPONSE);
            break;
        }
        case FSM_Enter(WAIT_FOR_RESPONSE):
        {
            break;
        }
        case FSM_Exit(WAIT_FOR_RESPONSE):
        {
            bool isFinished = processResponse(msg);
            if (isFinished)
            {
                FSObjectType objectType = createReq_->getObjectType();
                if (SPFS_DIR_ENT_OBJECT == objectType)
                {
                    FSM_Goto(currentState, SET_ATTR);
                }
                else
                {
                    FSM_Goto(currentState, FINISH);
                }
            }
            else
            {
                FSM_Goto(currentState, WAIT_FOR_RESPONSE);
            }
            break;
        }
        case FSM_Enter(SET_ATTR):
        {
            setAttributes();
            break;
        }
        case FSM_Exit(SET_ATTR):
        {
            FSM_Goto(currentState, CREATE_DIR_ENT);
            break;
        }
        case FSM_Enter(CREATE_DIR_ENT):
        {
            createDirEnt();
            break;
        }
        case FSM_Exit(CREATE_DIR_ENT):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            enterFinish();
            break;
        }
    }

    // Store the state in the request
    createReq_->setState(currentState);
}

void CollectiveCreate::sendMeta()
{
    spfsCollectiveCreateRequest* childCreate =
        new spfsCollectiveCreateRequest(0, SPFS_COLLECTIVE_CREATE_REQUEST);
    childCreate->setContextPointer(createReq_);
    childCreate->setObjectType(SPFS_METADATA_OBJECT);

    // The first child handle is used for addressing
    childCreate->setHandle(createReq_->getMetaHandle());
    childCreate->setByteLength(4 + 16 + 4 + 8);
    module_->send(childCreate);
}

void CollectiveCreate::enterCreate()
{
    // Create the file system object
    spfsOSFileOpenRequest* openRequest =
        new spfsOSFileOpenRequest(0, SPFS_OS_FILE_OPEN_REQUEST);
    openRequest->setContextPointer(createReq_);

    // Extract the handle as the file name
    Filename f(createReq_->getHandle());
    openRequest->setFilename(f.c_str());
    openRequest->setIsCreate(true);
    
    // Send the request to the storage layer
    module_->send(openRequest);

    // Increment the number of outstanding requests
    int numOutstanding = createReq_->getNumOutstandingRequests() + 1;
    createReq_->setNumOutstandingRequests(numOutstanding);
}

void CollectiveCreate::setAttributes()
{
    spfsSetAttrRequest* setAttr =
        new spfsSetAttrRequest(0, SPFS_SET_ATTR_REQUEST);
    setAttr->setContextPointer(createReq_);
    
    // The meta handle is used for addressing
    setAttr->setHandle(createReq_->getMetaHandle());
    setAttr->setObjectType(SPFS_METADATA_OBJECT);
    setAttr->setByteLength(4 + 16 + 4 + 8 + 4 + 4 + 64);
    module_->sendDelayed(setAttr, 0.0);
}

void CollectiveCreate::createDirEnt()
{
    // Convert the handle into a local file name
    Filename filename(createReq_->getHandle());

    // Create the file write request
    spfsOSFileWriteRequest* fileWrite = new spfsOSFileWriteRequest();
    fileWrite->setContextPointer(createReq_);
    fileWrite->setFilename(filename.c_str());
    fileWrite->setOffsetArraySize(1);
    fileWrite->setExtentArraySize(1);
    fileWrite->setOffset(0, 0);
    fileWrite->setExtent(0, module_->getDirectoryEntrySize());
    
    // Send the write request
    module_->send(fileWrite);
}

void CollectiveCreate::sendCollectiveRequests()
{
    // Divide the list of data handles into two partitions
    //   Send the second partition to another server
    //   Repeat process with first half of the list
    int numRemainingHandles = createReq_->getDataHandlesArraySize(); 
    while (0 < numRemainingHandles)
    {
        // Determine the next partition
        int splitIdx = numRemainingHandles / 2;
        int numChildHandles = numRemainingHandles - splitIdx;

        // Send the child collective create call
        spfsCollectiveCreateRequest* childCreate =
            createChildCollectiveRequest(splitIdx, numChildHandles);
        childCreate->setContextPointer(createReq_);
        module_->send(childCreate);

        // Increment the number of outstanding requests
        int numOutstanding = createReq_->getNumOutstandingRequests() + 1;
        createReq_->setNumOutstandingRequests(numOutstanding);

        // Adjust the number of unprocessed handles
        numRemainingHandles -= numChildHandles;
    }
}

bool CollectiveCreate::processResponse(cMessage* response)
{
    if (0 != dynamic_cast<spfsOSFileOpenResponse*>(response))
    {
        module_->recordCreateObjectDiskDelay(response);
    }

    // Determine if any outstanding requests remain
    int numOutstanding = createReq_->getNumOutstandingRequests() - 1;
    createReq_->setNumOutstandingRequests(numOutstanding);
    assert(0 <= numOutstanding);
    return (0 == numOutstanding);
}

void CollectiveCreate::enterFinish()
{
    spfsCollectiveCreateResponse* resp =
        new spfsCollectiveCreateResponse(0, SPFS_COLLECTIVE_CREATE_RESPONSE);
    resp->setContextPointer(createReq_);
    resp->setByteLength(4);

    // Determine the processing delay
    simtime_t delay = 0.0;
    if (SPFS_DATA_OBJECT == createReq_->getObjectType())
    {
        delay = FSServer::createDFileProcessingDelay();
    }
    else if (SPFS_DIRECTORY_OBJECT == createReq_->getObjectType())
    {
        delay = FSServer::createDirectoryProcessingDelay();
    }
    else if (SPFS_METADATA_OBJECT == createReq_->getObjectType())
    {
        delay = FSServer::createMetadataProcessingDelay();
    }
    else
    {
        assert(SPFS_DIR_ENT_OBJECT == createReq_->getObjectType());
        delay = FSServer::createDirEntProcessingDelay();
    }

    // Send the message after calculated delay
    module_->sendDelayed(resp, delay);
}

spfsCollectiveCreateRequest*
CollectiveCreate::createChildCollectiveRequest(int idx, int numHandles) const
{
    assert(0 < numHandles);
    
    spfsCollectiveCreateRequest* childCreate =
        new spfsCollectiveCreateRequest(0, SPFS_COLLECTIVE_CREATE_REQUEST);
    childCreate->setObjectType(SPFS_DATA_OBJECT);

    // The first child handle is used for addressing
    childCreate->setHandle(createReq_->getDataHandles(idx));
    idx++;
    numHandles--;
    
    // The remaining handles are sent as data
    childCreate->setDataHandlesArraySize(numHandles);
    for (int i = 0; i < numHandles; i++)
    {
        childCreate->setDataHandles(i, createReq_->getDataHandles(i + idx));
    }
    //cerr << "Child collective handles: "
    //     << childCreate->getDataHandlesArraySize() << endl;

    // Set the message size
    long msgSize = 4 + 16 + 4 + 4 + 8 + 4 + numHandles*4 + numHandles*8;
    childCreate->setByteLength(msgSize);
    return childCreate;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab cino=g0:
 */
