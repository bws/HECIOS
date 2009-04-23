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
