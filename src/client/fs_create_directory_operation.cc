//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_create_directory_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "file_builder.h"
#include "filename.h"
#include "fs_client.h"
#include "fs_create_directory_sm.h"
#include "fs_get_attributes_generic_sm.h"
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
