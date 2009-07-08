//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_read_operation.h"
#include "fs_client.h"
#include "fs_get_attributes_generic_sm.h"
#include "fs_read_sm.h"
#include "mpi_proto_m.h"

FSReadOperation::FSReadOperation(FSClient* client,
                                 spfsMPIFileReadAtRequest* readAtRequest)
    : FSClientOperation(readAtRequest),
      client_(client),
      readAtRequest_(readAtRequest)
{
    assert(0 != client_);
    assert(0 != readAtRequest_);
}

void FSReadOperation::registerStateMachines()
{
    // Retrieve the file attributes
    Filename file = readAtRequest_->getFileDes()->getFilename();
    addStateMachine(new FSGetAttributesSM(file,
                                          false,
                                          readAtRequest_,
                                          client_));

    // Perform the file read
    addStateMachine(new FSReadSM(readAtRequest_,
                                 client_));

}

void FSReadOperation::sendFinalResponse()
{
    spfsMPIFileReadAtResponse* mpiResp =
        new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
    mpiResp->setContextPointer(readAtRequest_);
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
