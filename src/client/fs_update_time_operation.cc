//
// This file is part of Hecios
//
// Copyright (C) 2008 bradles
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
#include "fs_update_time_operation.h"
#include <iostream>
#include <omnetpp.h>
#include "filename.h"
#include "fs_client.h"
#include "fs_get_attributes_sm.h"
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
