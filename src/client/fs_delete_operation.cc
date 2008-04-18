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
#include "fs_delete_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "filename.h"
#include "fs_client.h"
#include "fs_collective_remove_sm.h"
#include "fs_get_attributes_sm.h"
#include "fs_lookup_name_sm.h"
#include "fs_remove_sm.h"
#include "mpi_proto_m.h"
using namespace std;

FSDeleteOperation::FSDeleteOperation(FSClient* client,
                                     spfsMPIFileDeleteRequest* deleteReq,
                                     bool useCollectiveCommunication)
    : FSClientOperation(deleteReq),
      client_(client),
      deleteReq_(deleteReq),
      useCollectiveCommunication_(useCollectiveCommunication),
      deleteName_(deleteReq->getFileName())
{
    assert(0 != client_);
    assert(0 != deleteReq_);    
}

FSDeleteOperation::FSDeleteOperation(FSClient* client,
                                     spfsMPIDirectoryRemoveRequest* deleteReq,
                                     bool useCollectiveCommunication)
    : FSClientOperation(deleteReq),
      client_(client),
      deleteReq_(deleteReq),
      useCollectiveCommunication_(useCollectiveCommunication),
      deleteName_(deleteReq->getDirName())
{
    assert(0 != client_);
    assert(0 != deleteReq_);    
}

void FSDeleteOperation::registerStateMachines()
{
    // First - Lookup parent name
    Filename parentDir = deleteName_.getParent();
    addStateMachine(new FSLookupNameSM(parentDir, deleteReq_, client_));

    // Second - Lookup parent attributes
    addStateMachine(new FSGetAttributesSM(parentDir,
                                          false,
                                          deleteReq_,
                                          client_));

    // Finally - perform the file remove
    if (useCollectiveCommunication_)
    {
        addStateMachine(new FSCollectiveRemoveSM(deleteName_,
                                                 deleteReq_,
                                                 client_));
    }
    else
    {
        addStateMachine(new FSRemoveSM(deleteName_, deleteReq_, client_));
    }
}

void FSDeleteOperation::sendFinalResponse()
{
    cMessage* finalResponse = 0;
    if (SPFS_MPI_FILE_DELETE_REQUEST == deleteReq_->kind())
    {
        finalResponse = new spfsMPIFileDeleteResponse(
            0, SPFS_MPI_FILE_DELETE_RESPONSE);
    }
    else
    {
        assert(SPFS_MPI_DIRECTORY_REMOVE_REQUEST == deleteReq_->kind());
        finalResponse = new spfsMPIDirectoryRemoveResponse(
            0, SPFS_MPI_DIRECTORY_REMOVE_RESPONSE);
    }
    
    finalResponse->setContextPointer(deleteReq_);
    client_->send(finalResponse, client_->getAppOutGate());
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
