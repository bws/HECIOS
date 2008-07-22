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
#include "fs_write_operation.h"
#include "fs_client.h"
#include "fs_get_attributes_sm.h"
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
