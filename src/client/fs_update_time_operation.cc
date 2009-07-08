//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_update_time_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "filename.h"
#include "fs_client.h"
#include "fs_get_attributes_generic_sm.h"
#include "fs_set_attributes_sm.h"
#include "fs_lookup_name_sm.h"
#include "mpi_proto_m.h"
using namespace std;

FSUpdateTimeOperation::FSUpdateTimeOperation(FSClient* client,
                                             spfsMPIFileUpdateTimeRequest* utimeReq)
    : FSClientOperation(utimeReq),
      client_(client),
      updateTimeReq_(utimeReq)
{
    assert(0 != client_);
    assert(0 != updateTimeReq_);
}

void FSUpdateTimeOperation::registerStateMachines()
{
    // First - Lookup parent name
    Filename utimeFile(updateTimeReq_->getFileName());
    Filename parentDir = utimeFile.getParent();
    addStateMachine(new FSLookupNameSM(parentDir, updateTimeReq_, client_));

    // Second - Lookup parent attributes
    addStateMachine(new FSGetAttributesSM(parentDir,
                                          false,
                                          updateTimeReq_,
                                          client_));

    // Finally - update the time
    assert(true == fileExists(utimeFile));
    //addStateMachine(new FSLookupNameSM(utimeFile, updateTimeReq_, client_));
    addStateMachine(new FSSetAttributesSM(utimeFile,
                                          updateTimeReq_,
                                          client_));
}

void FSUpdateTimeOperation::sendFinalResponse()
{
    spfsMPIFileUpdateTimeResponse* mpiResp =
        new spfsMPIFileUpdateTimeResponse(0, SPFS_MPI_FILE_UPDATE_TIME_RESPONSE);
    mpiResp->setContextPointer(updateTimeReq_);
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
