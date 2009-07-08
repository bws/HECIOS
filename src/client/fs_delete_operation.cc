//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_delete_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "filename.h"
#include "fs_client.h"
#include "fs_collective_remove_sm.h"
#include "fs_get_attributes_generic_sm.h"
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
    // Check if the parent exists
    if (fileExists(parentDir))
    {
        addStateMachine(new FSGetAttributesSM(parentDir,
                                              false,
                                              deleteReq_,
                                              client_));

        // Finally - perform the file remove
        static bool printDiagnostic = true;
        if (useCollectiveCommunication_)
        {
            if (printDiagnostic)
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                    << "DIAGNOSTIC: Using collective remove "
                    << "(this diagnostic will not print again)\n";
                printDiagnostic = false;
            }
            addStateMachine(new FSCollectiveRemoveSM(deleteName_,
                                                     deleteReq_,
                                                     client_));
        }
        else
        {
            if (printDiagnostic)
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                    << "DIAGNOSTIC: Using serial remove "
                    << "(this diagnostic will not print again)\n";
                printDiagnostic = false;
            }
            addStateMachine(new FSRemoveSM(deleteName_, deleteReq_, client_));
        }
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
