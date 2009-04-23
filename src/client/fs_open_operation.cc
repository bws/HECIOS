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
#include "fs_open_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "file_builder.h"
#include "filename.h"
#include "fs_client.h"
#include "fs_collective_create_sm.h"
#include "fs_create_sm.h"
#include "fs_get_attributes_generic_sm.h"
#include "fs_lookup_name_sm.h"
#include "mpi_proto_m.h"
using namespace std;

FSOpenOperation::FSOpenOperation(FSClient* client,
                                 spfsMPIFileOpenRequest* openReq,
                                 bool useCollectiveCommunication)
    : FSClientOperation(openReq),
      client_(client),
      openReq_(openReq),
      useCollectiveCommunication_(useCollectiveCommunication)
{
    assert(0 != client_);
    assert(0 != openReq_);
}

void FSOpenOperation::registerStateMachines()
{
    // First - Lookup parent name
    Filename openFile(openReq_->getFileName());
    Filename parentDir = openFile.getParent();
    addStateMachine(new FSLookupNameSM(parentDir, openReq_, client_));

    // Check if the parent exists
    if (fileExists(parentDir))
    {
        // Second - Lookup parent attributes
        addStateMachine(new FSGetAttributesSM(parentDir, false, openReq_, client_));

        // Finally - Perform open/create
        if (isFileCreate())
        {
            static bool printDiagnostic = true;
            if (useCollectiveCommunication_)
            {
                addStateMachine(new FSCollectiveCreateSM(openFile,
                                                         openReq_,
                                                         client_));
                if (printDiagnostic)
                {
                    cerr << __FILE__ << ":" << __LINE__ << ":"
                        << "DIAGNOSTIC: Using collective create "
                        << "(this diagnostic will not print again)\n";
                    printDiagnostic = false;
                }
            }
            else
            {
                addStateMachine(new FSCreateSM(openFile, openReq_, client_));
                if (printDiagnostic)
                {
                    cerr << __FILE__ << ":" << __LINE__ << ":"
                        << "DIAGNOSTIC: Using serial create "
                        << "(this diagnostic will not print again)\n";
                    printDiagnostic = false;
                }
            }
        }
        else
        {
            addStateMachine(new FSLookupNameSM(openFile, openReq_, client_));
        }
    }
}

void FSOpenOperation::sendFinalResponse()
{
    spfsMPIFileOpenResponse *resp = new spfsMPIFileOpenResponse(
        0, SPFS_MPI_FILE_OPEN_RESPONSE);
    resp->setContextPointer(openReq_);
    resp->setFileDes(openReq_->getFileDes());
    client_->send(resp, client_->getAppOutGate());
}

bool FSOpenOperation::isFileCreate()
{
    if (openReq_->getMode() & MPI_MODE_CREATE)
    {
        return true;
    }
    else
    {
        return false;
    }
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
