//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_close_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "fs_client.h"
#include "mpi_proto_m.h"
using namespace std;

FSCloseOperation::FSCloseOperation(FSClient* client,
                                   spfsMPIFileCloseRequest* closeReq)
    : FSClientOperation(closeReq),
      client_(client),
      closeReq_(closeReq)
{
    assert(0 != client_);
    assert(0 != closeReq_);
}

void FSCloseOperation::registerStateMachines()
{
    // No state machines to register for this operation
}

void FSCloseOperation::sendFinalResponse()
{
    spfsMPIFileCloseResponse* mpiResp =
            new spfsMPIFileCloseResponse(0, SPFS_MPI_FILE_CLOSE_RESPONSE);
    mpiResp->setContextPointer(closeReq_);
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
