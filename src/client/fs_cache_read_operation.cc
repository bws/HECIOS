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
#include "fs_cache_read_operation.h"
#include "fs_client.h"
#include "fs_get_attributes_sm.h"
#include "fs_cache_read_sm.h"
#include "cache_proto_m.h"

FSCacheReadOperation::FSCacheReadOperation(FSClient* client,
                                           spfsCacheReadExclusiveRequest* readRequest)
    : FSOperation(readRequest->getOpState()),
      client_(client),
      readRequest_(readRequest),
      isExclusive_(true)
{
    assert(0 != client_);
    assert(0 != readRequest_);
    assert(isExclusive_);
}

FSCacheReadOperation::FSCacheReadOperation(FSClient* client,
                                           spfsCacheReadSharedRequest* readRequest)
    : FSOperation(readRequest->getOpState()),
      client_(client),
      readRequest_(readRequest),
      isExclusive_(false)
{
    assert(0 != client_);
    assert(0 != readRequest_);
    assert(!isExclusive_);
}

FSCacheReadOperation::~FSCacheReadOperation()
{
    // Store the updated operation state back into the read message
    readRequest_->setOpState(FSOperation::state());
}

void FSCacheReadOperation::registerStateMachines()
{
    spfsMPIFileRequest* mpiRequest =
        static_cast<spfsMPIFileRequest*>(readRequest_->contextPointer());

    // Retrieve the file attributes
    Filename file = mpiRequest->getFileDes()->getFilename();
    addStateMachine(new FSGetAttributesSM(file,
                                          false,
                                          mpiRequest,
                                          client_));

    // Perform the file read
    addStateMachine(new FSCacheReadSM(readRequest_,
                                      client_));

}

void FSCacheReadOperation::sendFinalResponse()
{
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
