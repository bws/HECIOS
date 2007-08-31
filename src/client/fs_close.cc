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
#include "fs_close.h"
#include <iostream>
#include <omnetpp.h>
#include "fs_module.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSClose::FSClose(fsModule* module, spfsMPIFileCloseRequest* closeReq)
    : fsModule_(module),
      closeReq_(closeReq)
{
    assert(0 != fsModule_);
    assert(0 != closeReq_);
}

// Processing that occurs upon receipt of an MPI-IO Open request
void FSClose::handleMessage(cMessage* msg)
{
    spfsMPIFileCloseResponse* mpiResp =
            new spfsMPIFileCloseResponse(0, SPFS_MPI_FILE_CLOSE_RESPONSE);
    mpiResp->setContextPointer(closeReq_);
    fsModule_->send(mpiResp, fsModule_->fsMpiOut);                 

}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
