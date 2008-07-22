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
#include "fs_create_directory_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "file_builder.h"
#include "filename.h"
#include "fs_client.h"
#include "fs_create_directory_sm.h"
#include "fs_get_attributes_sm.h"
#include "fs_lookup_name_sm.h"
#include "mpi_proto_m.h"

FSCreateDirectoryOperation::FSCreateDirectoryOperation(FSClient* client,
                                                       spfsMPIDirectoryCreateRequest* createRequest)
    : FSClientOperation(createRequest),
      client_(client),
      createDirReq_(createRequest)
{
    assert(0 != client_);
    assert(0 != createDirReq_);
}

void FSCreateDirectoryOperation::registerStateMachines()
{
    // First - Lookup parent name
    Filename createName(createDirReq_->getDirName());
    Filename parentDir = createName.getParent();
    addStateMachine(new FSLookupNameSM(parentDir, createDirReq_, client_));

    // Check if the parent exists
    if (fileExists(parentDir))
    {
        // Second - Lookup parent attributes
        addStateMachine(new FSGetAttributesSM(parentDir,
                                              false,
                                              createDirReq_,
                                              client_));

        // Finally - perform the directory creation
        addStateMachine(new FSCreateDirectorySM(createName,
                                                createDirReq_,
                                                client_));
    }
}

void FSCreateDirectoryOperation::sendFinalResponse()
{
    cMessage* finalResponse =
        new spfsMPIDirectoryCreateResponse(
           0, SPFS_MPI_DIRECTORY_CREATE_RESPONSE);
    finalResponse->setContextPointer(createDirReq_);
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
