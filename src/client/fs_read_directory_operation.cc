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
#include "fs_read_directory_operation.h"
#include "fs_client.h"
#include "fs_get_attributes_sm.h"
#include "fs_read_directory_sm.h"
#include "mpi_proto_m.h"

FSReadDirectoryOperation::FSReadDirectoryOperation(FSClient* client,
                                                   spfsMPIDirectoryReadRequest* readDirRequest)
    : FSClientOperation(readDirRequest),
      client_(client),
      dirReadRequest_(readDirRequest)
{
    assert(0 != client_);
    assert(0 != dirReadRequest_);
}

void FSReadDirectoryOperation::registerStateMachines()
{
    // Retrieve the file attributes
    Filename file = dirReadRequest_->getFileDes()->getFilename();
    addStateMachine(new FSGetAttributesSM(file,
                                          false,
                                          dirReadRequest_,
                                          client_));

    // Perform the file write
    addStateMachine(new FSReadDirectorySM(dirReadRequest_->getFileDes(),
                                          dirReadRequest_->getCount(),
                                          dirReadRequest_,
                                          client_));
}

void FSReadDirectoryOperation::sendFinalResponse()
{
     spfsMPIDirectoryReadResponse *resp = new spfsMPIDirectoryReadResponse(
         0, SPFS_MPI_DIRECTORY_READ_RESPONSE);
     resp->setContextPointer(dirReadRequest_);
     client_->send(resp, client_->getAppOutGate());

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
