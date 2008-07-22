//
// This file is part of Hecios
//
// Copyright (C) 2008 bradles
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
#include "fs_create_directory_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSCreateDirectorySM::FSCreateDirectorySM(const Filename& filename,
                                         spfsMPIRequest* mpiReq,
                                         FSClient* client)
    : createDirName_(filename),
      mpiReq_(mpiReq),
      client_(client)
{
}
bool FSCreateDirectorySM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system collective create state machine states */
    enum {
        INIT = 0,
        CREATE_META = FSM_Steady(1),
        CREATE_DATA = FSM_Steady(2),
        WRITE_ATTR = FSM_Steady(3),
        WRITE_DIRENT = FSM_Steady(4),
        FINISH = FSM_Steady(5)
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, CREATE_META);
            break;
        }
        case FSM_Enter(CREATE_META):
        {
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
            createDataObject();
            break;
        }
        case FSM_Exit(CREATE_DATA):
        {
            FSM_Goto(currentState, WRITE_ATTR);
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
        case FSM_Enter(FINISH):
        {
            updateClientCache();
            isComplete = true;
            break;
        }
    }

    return isComplete;
}
void FSCreateDirectorySM::createMeta()
{
    // hash path to meta server number
    int metaServer = FileBuilder::instance().getMetaServers()[0];

    // Build message to create metadata
    spfsCreateRequest* req = FSClient::createCreateRequest(
        FileBuilder::instance().getFirstHandle(metaServer),
        SPFS_METADATA_OBJECT);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSCreateDirectorySM::writeAttributes()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(createDirName_);
    spfsSetAttrRequest* req =
        FSClient::createSetAttrRequest(meta->handle, SPFS_METADATA_OBJECT);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSCreateDirectorySM::createDataObject()
{
    // Retrieve the bookkeeping information for this directory
    const FSMetaData* metaData =
        FileBuilder::instance().getMetaData(createDirName_);
    assert(0 != metaData);
    assert(1 == metaData->dataHandles.size());

    // Construct the create requests for this directory's data handles
    spfsCreateRequest* create =
        FSClient::createCreateRequest(metaData->dataHandles[0],
                                      SPFS_DIRECTORY_OBJECT);
    create->setContextPointer(mpiReq_);
    client_->send(create, client_->getNetOutGate());
}

void FSCreateDirectorySM::createDirEnt()
{
    // Get the parent handle
    Filename parentName = createDirName_.getParent();
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);

    // Construct the directory entry creation request
    spfsCreateDirEntRequest *req =
        FSClient::createCreateDirEntRequest(parentMeta->handle,
                                            createDirName_);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSCreateDirectorySM::updateClientCache()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(createDirName_);

     // Update the name cache
     client_->fsState().insertName(createDirName_.str(), meta->handle);

     // Update the attr cache
     client_->fsState().insertAttr(meta->handle, *meta);
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
