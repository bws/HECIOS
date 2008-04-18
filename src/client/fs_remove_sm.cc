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
#include "fs_remove_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSRemoveSM::FSRemoveSM(const Filename& filename,
                       spfsMPIRequest* mpiReq,
                       FSClient* client)
    : removeName_(filename),
      mpiReq_(mpiReq),
      client_(client)
{
}

bool FSRemoveSM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system remove state machine states */
    enum {
        INIT = 0,
        REMOVE_DIRENT = FSM_Steady(1),
        REMOVE_META = FSM_Steady(2),
        REMOVE_DATA = FSM_Transient(3),
        COUNT_REMOVE_RESPONSES = FSM_Steady(4),
        FINISH = FSM_Steady(5)
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "DIAGNOSTIC: Using serial remove\n";
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
            assert(0 != dynamic_cast<spfsRemoveDirEntResponse*>(msg));
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
            removeDataObjects();
            break;
        }
        case FSM_Exit(REMOVE_DATA):
        {
            FSM_Goto(currentState, COUNT_REMOVE_RESPONSES);
            break;
        }
        case FSM_Exit(COUNT_REMOVE_RESPONSES):
        {
            assert(0 != dynamic_cast<spfsRemoveResponse*>(msg));
            bool isDataRemovalComplete = countDataRemoveResponse();
            if (isDataRemovalComplete)
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT_REMOVE_RESPONSES);
            }
            break;
        }
        case FSM_Enter(FINISH):
        {
            removeNameAndAttributesFromCache();
            isComplete = true;
            break;
        }
    }

    return isComplete;
}

void FSRemoveSM::removeDirEnt()
{
    Filename parentName =  removeName_.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);

    spfsRemoveDirEntRequest* removeDirEnt =
        FSClient::createRemoveDirEntRequest(parentMeta->handle, removeName_);
    removeDirEnt->setContextPointer(mpiReq_);
    client_->send(removeDirEnt, client_->getNetOutGate());
}

void FSRemoveSM::removeMeta()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(removeName_);

    spfsRemoveRequest* remove =
        FSClient::createRemoveRequest(meta->handle, SPFS_METADATA_OBJECT);
    remove->setContextPointer(mpiReq_);
    client_->send(remove, client_->getNetOutGate());
}

void FSRemoveSM::removeDataObjects()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(removeName_);

    // Schedule the remove messages
    for (size_t i = 0; i < meta->dataHandles.size(); i++)
    {
        spfsRemoveRequest* remove =
            FSClient::createRemoveRequest(meta->dataHandles[i],
                                          SPFS_DATA_OBJECT);
        remove->setContextPointer(mpiReq_);
        client_->send(remove, client_->getNetOutGate());
    }

    // Initialize the response counter
    mpiReq_->setRemainingResponses(meta->dataHandles.size());
}

bool FSRemoveSM::countDataRemoveResponse()
{
    // Decrement the number of create responses to expect
    int numResponses = mpiReq_->getRemainingResponses() - 1;
    mpiReq_->setRemainingResponses(numResponses);

    bool isComplete = false;
    if (0 == numResponses)
    {
        isComplete = true;
    }
    return isComplete;
}

void FSRemoveSM::removeNameAndAttributesFromCache()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(removeName_);
    client_->fsState().removeName(removeName_.str());
    client_->fsState().removeAttr(meta->handle);
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
