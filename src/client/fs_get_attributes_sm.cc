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
#include "fs_get_attributes_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSGetAttributesSM::FSGetAttributesSM(const Filename& filename,
                                     bool calculateSize,
                                     spfsMPIRequest* mpiRequest,
                                     FSClient* client)
    : handle_(FileBuilder::instance().getMetaData(filename)->handle),
      calculateSize_(calculateSize),
      mpiReq_(mpiRequest),
      client_(client)
{
    assert(0 != client_);
    assert(0 != mpiReq_);
}

bool FSGetAttributesSM::updateState(cFSM& currentState, cMessage* msg)
{
    // File system get attributes state machine states
    enum {
        INIT = 0,
        GET_META_ATTR = FSM_Steady(1),
        GET_DATA_ATTR = FSM_Transient(2),
        COUNT_RESPONSES = FSM_Steady(3),
        FINISH = FSM_Steady(4),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            bool isCached = isAttrCached();
            if (isCached && !calculateSize_)
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, GET_META_ATTR);
            }
            break;
        }
        case FSM_Enter(GET_META_ATTR):
        {
            getMetadata();
            break;
        }
        case FSM_Exit(GET_META_ATTR):
        {
            if (calculateSize_)
            {
                FSM_Goto(currentState, GET_DATA_ATTR);
            }
            else
            {
                FSM_Goto(currentState, FINISH);
            }
            break;
        }
        case FSM_Enter(GET_DATA_ATTR):
        {
            getDataAttributes();
            break;
        }
        case FSM_Exit(GET_DATA_ATTR):
        {
            FSM_Goto(currentState, COUNT_RESPONSES);
            break;
        }
        case FSM_Exit(COUNT_RESPONSES):
        {
            bool isFinished = countResponse();
            if (isFinished)
                FSM_Goto(currentState, FINISH);
            else
                FSM_Goto(currentState, COUNT_RESPONSES);
            break;
        }        
         case FSM_Enter(FINISH):
        {
            cacheAttributes();
            isComplete = true;
            break;
        }
    }
    return isComplete;
}

bool FSGetAttributesSM::isAttrCached()
{
    FSMetaData* lookup = client_->fsState().lookupAttr(handle_);
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

void FSGetAttributesSM::getMetadata()
{
    // Construct the request
    spfsGetAttrRequest *req =
        FSClient::createGetAttrRequest(handle_, SPFS_METADATA_OBJECT);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSGetAttributesSM::getDataAttributes()
{
    const FSMetaData* meta = FileBuilder::instance().getMetaData(handle_);

    for (size_t i = 0; i < meta->dataHandles.size(); i++)
    {
        spfsGetAttrRequest* req = FSClient::createGetAttrRequest(
            meta->dataHandles[i], SPFS_DATA_OBJECT);
        req->setContextPointer(mpiReq_);
        client_->send(req, client_->getNetOutGate());
    }

    mpiReq_->setRemainingResponses(meta->dataHandles.size());
}

bool FSGetAttributesSM::countResponse()
{
    bool isComplete = false;

    int numOutstanding = mpiReq_->getRemainingResponses() - 1;
    mpiReq_->setRemainingResponses(numOutstanding);
    if (0 == numOutstanding)
    {
        isComplete = true;
    }
    return isComplete;
}

void FSGetAttributesSM::cacheAttributes()
{
    const FSMetaData* attr = FileBuilder::instance().getMetaData(handle_);
    client_->fsState().insertAttr(handle_, *attr);
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
