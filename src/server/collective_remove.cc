//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#define FSM_DEBUG  // Enable FSM Debug output
#include <cassert>
#include <omnetpp.h>
#include "collective_remove.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

CollectiveRemove::CollectiveRemove(FSServer* module,
                                   spfsCollectiveRemoveRequest* removeReq)
    : module_(module),
      removeReq_(removeReq)
{
}

void CollectiveRemove::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = removeReq_->getState();

    // Server remove states
    enum {
        INIT = 0,
        SEND_META = FSM_Steady(1),
        REMOVE_META = FSM_Steady(2),
        REMOVE_DFILE = FSM_Transient(3),
        SEND_REQUESTS = FSM_Transient(4),
        WAIT_FOR_RESPONSE = FSM_Steady(5),
        REMOVE_DIRENT = FSM_Steady(6),
        FINISH = FSM_Steady(7),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsCollectiveRemoveRequest*>(msg));
            module_->recordCollectiveCreate();
            removeReq_->setNumOutstandingRequests(0);
            FSObjectType objectType = removeReq_->getObjectType();
            if (SPFS_DIR_ENT_OBJECT == objectType)
            {
                FSM_Goto(currentState, SEND_META);
            }
            else if (SPFS_METADATA_OBJECT == objectType)
            {
                FSM_Goto(currentState, REMOVE_META);
            }
            else
            {
                assert(SPFS_DATA_OBJECT == objectType);
                FSM_Goto(currentState, REMOVE_DFILE);
            }
            break;
        }
        case FSM_Enter(SEND_META):
        {
            assert(0 != dynamic_cast<spfsCollectiveRemoveRequest*>(msg));
            sendRemoveMeta();
            break;
        }
        case FSM_Exit(SEND_META):
        {
            FSM_Goto(currentState, SEND_REQUESTS);
            break;
        }
        case FSM_Enter(REMOVE_META):
        {
            assert(0 != dynamic_cast<spfsCollectiveRemoveRequest*>(msg));
            removeObject();
            break;
        }
        case FSM_Exit(REMOVE_META):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(REMOVE_DFILE):
        {
            assert(0 != dynamic_cast<spfsCollectiveRemoveRequest*>(msg));
            removeObject();
            break;
        }
        case FSM_Exit(REMOVE_DFILE):
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
                FSObjectType objectType = removeReq_->getObjectType();
                if (SPFS_DIR_ENT_OBJECT == objectType)
                {
                    FSM_Goto(currentState, REMOVE_DIRENT);
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
        case FSM_Enter(REMOVE_DIRENT):
        {
            removeDirEnt();
            break;
        }
        case FSM_Exit(REMOVE_DIRENT):
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
    removeReq_->setState(currentState);
}

void CollectiveRemove::sendRemoveMeta()
{
    spfsCollectiveRemoveRequest* childRemove =
        new spfsCollectiveRemoveRequest(0, SPFS_COLLECTIVE_REMOVE_REQUEST);
    childRemove->setObjectType(SPFS_METADATA_OBJECT);
    childRemove->setHandle(removeReq_->getMetaHandle());
    childRemove->setContextPointer(removeReq_);
    childRemove->setByteLength(4 + 16 + 4 + 8);
    module_->send(childRemove);
}

void CollectiveRemove::removeObject()
{
    // Create the file system object
    spfsOSFileUnlinkRequest* unlinkRequest =
        new spfsOSFileUnlinkRequest(0, SPFS_OS_FILE_OPEN_REQUEST);
    unlinkRequest->setContextPointer(removeReq_);

    // Extract the handle as the file name
    Filename f(removeReq_->getHandle());
    unlinkRequest->setFilename(f.c_str());

    // Send the request to the storage layer
    module_->send(unlinkRequest);

    // Increment the number of outstanding requests
    int numOutstanding = removeReq_->getNumOutstandingRequests() + 1;
    removeReq_->setNumOutstandingRequests(numOutstanding);
}

void CollectiveRemove::removeDirEnt()
{
    // Convert the handle into a local file name
    Filename filename(removeReq_->getHandle());

    // Create the file write request
    spfsOSFileWriteRequest* fileWrite = new spfsOSFileWriteRequest();
    fileWrite->setContextPointer(removeReq_);
    fileWrite->setFilename(filename.c_str());
    fileWrite->setOffsetArraySize(1);
    fileWrite->setExtentArraySize(1);
    fileWrite->setOffset(0, 0);
    fileWrite->setExtent(0, module_->getDirectoryEntrySize());

    // Send the write request
    module_->send(fileWrite);
}

void CollectiveRemove::sendCollectiveRequests()
{
    // Divide the list of data handles into two partitions
    //   Send the second partition to another server
    //   Repeat process with first half of the list
    int numRemainingHandles = removeReq_->getDataHandlesArraySize();
    while (0 < numRemainingHandles)
    {
        // Determine the next partition
        int splitIdx = numRemainingHandles / 2;
        int numChildHandles = numRemainingHandles - splitIdx;

        // Send the child collective remove call
        spfsCollectiveRemoveRequest* childRemove =
            createChildCollectiveRequest(splitIdx, numChildHandles);
        childRemove->setContextPointer(removeReq_);
        module_->send(childRemove);

        // Increment the number of outstanding requests
        int numOutstanding = removeReq_->getNumOutstandingRequests() + 1;
        removeReq_->setNumOutstandingRequests(numOutstanding);

        // Adjust the number of unprocessed handles
        numRemainingHandles -= numChildHandles;
    }
}

bool CollectiveRemove::processResponse(cMessage* response)
{
    if (0 != dynamic_cast<spfsOSFileUnlinkResponse*>(response))
    {
        module_->recordRemoveObjectDiskDelay(response);
    }

    // Determine if any outstanding requests remain
    int numOutstanding = removeReq_->getNumOutstandingRequests() - 1;
    removeReq_->setNumOutstandingRequests(numOutstanding);
    assert(0 <= numOutstanding);
    return (0 == numOutstanding);
}

void CollectiveRemove::enterFinish()
{
    spfsCollectiveRemoveResponse* resp =
        new spfsCollectiveRemoveResponse(0, SPFS_COLLECTIVE_REMOVE_RESPONSE);
    resp->setContextPointer(removeReq_);
    resp->setByteLength(4);

    // Determine the processing delay
    simtime_t delay = 0.0;
    if (SPFS_DATA_OBJECT == removeReq_->getObjectType())
    {
        delay = FSServer::removeObjectProcessingDelay();
    }
    else if (SPFS_DIRECTORY_OBJECT == removeReq_->getObjectType())
    {
        delay = FSServer::removeObjectProcessingDelay();
    }
    else if (SPFS_METADATA_OBJECT == removeReq_->getObjectType())
    {
        delay = FSServer::removeObjectProcessingDelay();
    }
    else
    {
        assert(SPFS_DIR_ENT_OBJECT == removeReq_->getObjectType());
        delay = FSServer::removeDirEntProcessingDelay();
    }

    // Send the message after calculated delay
    module_->sendDelayed(resp, delay);
}

spfsCollectiveRemoveRequest*
CollectiveRemove::createChildCollectiveRequest(int idx, int numHandles) const
{
    assert(0 < numHandles);

    spfsCollectiveRemoveRequest* childRemove =
        new spfsCollectiveRemoveRequest(0, SPFS_COLLECTIVE_REMOVE_REQUEST);
    childRemove->setObjectType(SPFS_DATA_OBJECT);

    // The first child handle is used for addressing
    childRemove->setHandle(removeReq_->getDataHandles(idx));
    idx++;
    numHandles--;

    // The remaining handles are sent as data
    childRemove->setDataHandlesArraySize(numHandles);
    for (int i = 0; i < numHandles; i++)
    {
        childRemove->setDataHandles(i, removeReq_->getDataHandles(i + idx));
    }
    //cerr << "Child collective handles: "
    //     << childCreate->getDataHandlesArraySize() << endl;

    // Set the message size
    long msgSize = 4 + 16 + 4 + 4 + 8 + 4 + numHandles*4 + numHandles*8;
    childRemove->setByteLength(msgSize);
    return childRemove;
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
