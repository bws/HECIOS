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
#include "fs_read_operation.h"
#include "fs_client.h"
#include "fs_get_attributes_sm.h"
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
