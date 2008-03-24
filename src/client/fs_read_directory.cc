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
#include "fs_read_directory.h"
#include <iostream>
#include <omnetpp.h>
#include "client_fs_state.h"
#include "filename.h"
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSReadDirectory::FSReadDirectory(FSClient* client,
                                 spfsMPIDirectoryReadRequest* readReq)
    : client_(client),
      readReq_(readReq)
{
    assert(0 != client_);
    assert(0 != readReq_);
}

// Processing that occurs upon receipt of an MPI-IO Directory Read request
void FSReadDirectory::handleMessage(cMessage* msg)
{
    // Restore the existing state for this Directory Create Request
    cFSM currentState = readReq_->getState();

    // File system open state machine states
    enum {
        INIT = 0,
        GET_ATTRIBUTES = FSM_Steady(1),
        READ_DIR_ENTS = FSM_Steady(2),
        FINISH = FSM_Steady(3)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, GET_ATTRIBUTES);
            break;
        }
        case FSM_Enter(GET_ATTRIBUTES):
        {
            getAttributes();
            break;
        }
        case FSM_Exit(GET_ATTRIBUTES):
        {
            addAttributesToCache();
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(READ_DIR_ENTS):
        {
            readDirEnt();
            break;
        }
        case FSM_Exit(READ_DIR_ENTS):
        {
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
    readReq_->setState(currentState);
}

void FSReadDirectory::getAttributes()
{
    FileDescriptor* fd = readReq_->getFileDes();
    const FSMetaData* meta = fd->getMetaData();

    spfsGetAttrRequest* req = FSClient::createGetAttrRequest(
        meta->handle, SPFS_DIRECTORY_OBJECT);
    req->setContextPointer(readReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSReadDirectory::addAttributesToCache()
{
    FileDescriptor* fd = readReq_->getFileDes();
    const FSMetaData* meta = fd->getMetaData();
    client_->fsState().insertAttr(meta->handle, *meta);
}

void FSReadDirectory::readDirEnt()
{
    FileDescriptor* fd = readReq_->getFileDes();
    const FSMetaData* meta = fd->getMetaData();

    spfsReadDirRequest* req = FSClient::createReadDirRequest(
        meta->dataHandles[0], readReq_->getCount());
    req->setContextPointer(readReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSReadDirectory::finish()
{
    spfsMPIDirectoryReadResponse *resp = new spfsMPIDirectoryReadResponse(
        0, SPFS_MPI_DIRECTORY_READ_RESPONSE);
    resp->setContextPointer(readReq_);
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
