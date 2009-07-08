//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_stat_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "filename.h"
#include "fs_client.h"
#include "fs_collective_get_attributes_sm.h"
#include "fs_get_attributes_generic_sm.h"
#include "fs_lookup_name_sm.h"
#include "mpi_proto_m.h"
using namespace std;

FSStatOperation::FSStatOperation(FSClient* client,
                                 spfsMPIFileStatRequest* statReq,
                                 bool useCollectiveCommunication)
    : FSClientOperation(statReq),
      client_(client),
      statReq_(statReq),
      useCollectiveCommunication_(useCollectiveCommunication)
{
    assert(0 != client_);
    assert(0 != statReq_);
}

void FSStatOperation::registerStateMachines()
{
    // First - Lookup parent name
    Filename statFile(statReq_->getFileName());
    Filename parentDir = statFile.getParent();
    addStateMachine(new FSLookupNameSM(parentDir, statReq_, client_));

    // Second - Lookup parent attributes
    addStateMachine(new FSGetAttributesSM(parentDir,
                                          false,
                                          statReq_,
                                          client_));

    // Finally - perform the file stat
    addStateMachine(new FSLookupNameSM(statFile, statReq_, client_));
    static bool printDiagnostic = true;
    if (useCollectiveCommunication_)
    {
        if (printDiagnostic)
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                << "DIAGNOSTIC: Using collective stat "
                << "(this diagnostic will not print again)\n";
            printDiagnostic = false;
        }
        addStateMachine(new FSCollectiveGetAttributesSM(statFile,
                                                        true,
                                                        statReq_,
                                                        client_));
    }
    else
    {
        if (printDiagnostic)
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                << "DIAGNOSTIC: Using serial stat "
                << "(this diagnostic will not print again)\n";
            printDiagnostic = false;
        }
        addStateMachine(new FSGetAttributesSM(statFile,
                                              true,
                                              statReq_,
                                              client_));
    }
}

void FSStatOperation::sendFinalResponse()
{
    spfsMPIFileStatResponse* mpiResp =
        new spfsMPIFileStatResponse(0, SPFS_MPI_FILE_STAT_RESPONSE);
    mpiResp->setContextPointer(statReq_);
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
