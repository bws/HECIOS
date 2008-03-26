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
#include "collective_get_attr.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

CollectiveGetAttr::CollectiveGetAttr(FSServer* module,
                                     spfsCollectiveGetAttrRequest* getAttrReq)
    : module_(module),
      getAttrReq_(getAttrReq)
{
}

void CollectiveGetAttr::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = getAttrReq_->getState();

    // Server create states
    enum {
        INIT = 0,
        GET_META_ATTR = FSM_Transient(1),
        GET_DFILE_ATTR = FSM_Transient(2),
        SEND_REQUESTS = FSM_Transient(3),
        WAIT_FOR_RESPONSE = FSM_Steady(4),
        FINISH = FSM_Steady(5),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsCollectiveGetAttrRequest*>(msg));
            module_->recordCollectiveGetAttr();
            getAttrReq_->setNumOutstandingRequests(0);
            FSObjectType objectType = getAttrReq_->getObjectType();
            if (SPFS_METADATA_OBJECT == objectType)
            {
                FSM_Goto(currentState, GET_META_ATTR);
            }
            else
            {
                assert(SPFS_DATA_OBJECT == objectType);
                FSM_Goto(currentState, GET_DFILE_ATTR);
            }
            break;
        }
        case FSM_Enter(GET_META_ATTR):
        {
            assert(0 != dynamic_cast<spfsCollectiveGetAttrRequest*>(msg));
            getLocalAttributes();
            break;
        }
        case FSM_Exit(GET_META_ATTR):
        {
            FSM_Goto(currentState, SEND_REQUESTS);
            break;
        }
        case FSM_Enter(GET_DFILE_ATTR):
        {
            assert(0 != dynamic_cast<spfsCollectiveGetAttrRequest*>(msg));
            getLocalAttributes();
            break;
        }
        case FSM_Exit(GET_DFILE_ATTR):
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
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, WAIT_FOR_RESPONSE);
            }
            break;
        }
        case FSM_Enter(FINISH):
        {
            enterFinish();
            break;
        }
    }

    // Store the state in the request
    getAttrReq_->setState(currentState);
}

void CollectiveGetAttr::getLocalAttributes()
{
    // Extract the handle as the file name
    Filename f(getAttrReq_->getHandle());

    // Create the file system object
    spfsOSFileReadRequest* readRequest = new spfsOSFileReadRequest();
    readRequest->setContextPointer(getAttrReq_);

    // Set read parameters to accomplish a get attr
    readRequest->setFilename(f.c_str());
    readRequest->setOffsetArraySize(1);
    readRequest->setExtentArraySize(1);
    readRequest->setOffset(0, 0);
    readRequest->setExtent(0, module_->getDefaultAttrSize());
    
    // Send the request to the storage layer
    module_->send(readRequest);

    // Increment the number of outstanding requests
    int numOutstanding = getAttrReq_->getNumOutstandingRequests() + 1;
    getAttrReq_->setNumOutstandingRequests(numOutstanding);
}

void CollectiveGetAttr::sendCollectiveRequests()
{
    // Divide the list of data handles into two partitions
    //   Send the second partition to another server
    //   Repeat process with first half of the list
    int numRemainingHandles = getAttrReq_->getDataHandlesArraySize(); 
    while (0 < numRemainingHandles)
    {
        // Determine the next partition
        int splitIdx = numRemainingHandles / 2;
        int numChildHandles = numRemainingHandles - splitIdx;

        // Send the child collective get attr call
        spfsCollectiveGetAttrRequest* childGetAttr =
            createChildCollectiveRequest(splitIdx, numChildHandles);
        childGetAttr->setContextPointer(getAttrReq_);
        module_->send(childGetAttr);

        // Increment the number of outstanding requests
        int numOutstanding = getAttrReq_->getNumOutstandingRequests() + 1;
        getAttrReq_->setNumOutstandingRequests(numOutstanding);

        // Adjust the number of unprocessed handles
        numRemainingHandles -= numChildHandles;
    }
}

bool CollectiveGetAttr::processResponse(cMessage* response)
{
    if (0 != dynamic_cast<spfsOSFileOpenResponse*>(response))
    {
        module_->recordCreateObjectDiskDelay(response);
    }

    // Determine if any outstanding requests remain
    int numOutstanding = getAttrReq_->getNumOutstandingRequests() - 1;
    getAttrReq_->setNumOutstandingRequests(numOutstanding);
    assert(0 <= numOutstanding);
    return (0 == numOutstanding);
}

void CollectiveGetAttr::enterFinish()
{
    spfsCollectiveGetAttrResponse* resp =
        new spfsCollectiveGetAttrResponse(0, SPFS_COLLECTIVE_GET_ATTR_RESPONSE);
    resp->setContextPointer(getAttrReq_);
    resp->setByteLength(4);

    // Determine the processing delay
    simtime_t delay = 0.0;

    // Send the message after calculated delay
    module_->sendDelayed(resp, delay);
}

spfsCollectiveGetAttrRequest*
CollectiveGetAttr::createChildCollectiveRequest(int idx, int numHandles) const
{
    assert(0 < numHandles);
    
    spfsCollectiveGetAttrRequest* childGetAttr =
        new spfsCollectiveGetAttrRequest(0, SPFS_COLLECTIVE_GET_ATTR_REQUEST);
    childGetAttr->setObjectType(SPFS_DATA_OBJECT);

    // The first child handle is used for addressing
    childGetAttr->setHandle(getAttrReq_->getDataHandles(idx));
    idx++;
    numHandles--;
    
    // The remaining handles are sent as data
    childGetAttr->setDataHandlesArraySize(numHandles);
    for (int i = 0; i < numHandles; i++)
    {
        childGetAttr->setDataHandles(i, getAttrReq_->getDataHandles(i + idx));
    }
    //cerr << "Child collective handles: "
    //     << childCreate->getDataHandlesArraySize() << endl;

    // Set the message size
    long msgSize = 4 + 16 + 4 + 4 + 8 + 4 + numHandles*4 + numHandles*8;
    childGetAttr->setByteLength(msgSize);
    return childGetAttr;
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
