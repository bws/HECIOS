//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_write_operation.h"
#include "fs_client.h"
#include "fs_get_attributes_generic_sm.h"
#include "fs_write_sm.h"
#include "mpi_proto_m.h"

FSWriteOperation::FSWriteOperation(FSClient* client,
                                   spfsMPIFileWriteAtRequest* writeAtRequest)
    : FSClientOperation(writeAtRequest),
      client_(client),
      writeAtRequest_(writeAtRequest)
{
    assert(0 != client_);
    assert(0 != writeAtRequest_);
}

void FSWriteOperation::registerStateMachines()
{
    // Retrieve the file attributes
    Filename file = writeAtRequest_->getFileDes()->getFilename();
    addStateMachine(new FSGetAttributesSM(file,
                                          false,
                                          writeAtRequest_,
                                          client_));

    // Perform the file write
    addStateMachine(new FSWriteSM(writeAtRequest_,
                                  client_));
}

void FSWriteOperation::sendFinalResponse()
{
    spfsMPIFileWriteAtResponse* mpiResp =
        new spfsMPIFileWriteAtResponse(0, SPFS_MPI_FILE_WRITE_AT_RESPONSE);
    mpiResp->setContextPointer(writeAtRequest_);
    mpiResp->setIsSuccessful(true);
    client_->send(mpiResp, client_->getAppOutGate());
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
