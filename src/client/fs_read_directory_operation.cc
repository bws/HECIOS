//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_read_directory_operation.h"
#include "fs_client.h"
#include "fs_get_attributes_generic_sm.h"
#include "fs_read_directory_sm.h"
#include "mpi_proto_m.h"

FSReadDirectoryOperation::FSReadDirectoryOperation(FSClient* client,
                                                   spfsMPIDirectoryReadRequest* readDirRequest)
    : FSClientOperation(readDirRequest),
      client_(client),
      dirReadRequest_(readDirRequest)
{
    assert(0 != client_);
    assert(0 != dirReadRequest_);
}

void FSReadDirectoryOperation::registerStateMachines()
{
    // Retrieve the file attributes
    Filename file = dirReadRequest_->getFileDes()->getFilename();
    addStateMachine(new FSGetAttributesSM(file,
                                          false,
                                          dirReadRequest_,
                                          client_));

    // Perform the file write
    addStateMachine(new FSReadDirectorySM(dirReadRequest_->getFileDes(),
                                          dirReadRequest_->getCount(),
                                          dirReadRequest_,
                                          client_));
}

void FSReadDirectoryOperation::sendFinalResponse()
{
     spfsMPIDirectoryReadResponse *resp = new spfsMPIDirectoryReadResponse(
         0, SPFS_MPI_DIRECTORY_READ_RESPONSE);
     resp->setContextPointer(dirReadRequest_);
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
