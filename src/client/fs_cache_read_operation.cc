//
// This file is part of Hecios
//
// Copyright (C) 2008 Bradley W. Settlemyer
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
#include <algorithm>
#include <cstddef>
#include <iostream>
#include "file_page.h"
#include "fs_client.h"
#include "fs_get_attributes_generic_sm.h"
#include "fs_cache_read_sm.h"
#include "cache_proto_m.h"
using namespace std;

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
    addStateMachine(new FSGetAttributesGenericSM<spfsCacheReadRequest>(file,
                                                                false,
                                                                readRequest_,
                                                                client_));

    // Perform the file read
    addStateMachine(new FSCacheReadSM(readRequest_,
                                      client_,
                                      isExclusive_));
}

void FSCacheReadOperation::sendFinalResponse()
{
    spfsCacheReadResponse* readResponse;

    if (isExclusive_)
    {
        readResponse =
            new spfsCacheReadExclusiveResponse(0, SPFS_CACHE_READ_EXCLUSIVE_RESPONSE);
    }
    else
    {
        readResponse =
            new spfsCacheReadSharedResponse(0, SPFS_CACHE_READ_SHARED_RESPONSE);
    }
    readResponse->setContextPointer(readRequest_);

    // Set the response's filename
    FileDescriptor* fd = readRequest_->getDescriptor();
    readResponse->setFilename(fd->getFilename().c_str());

    // Set the page ids
    setServerPageIds(readResponse);

    client_->send(readResponse, client_->getAppOutGate());
}

void FSCacheReadOperation::setServerPageIds(spfsCacheReadResponse* readResponse)
{
    // Get the full requested set of page ids
    set<FilePageId> requestedPages;
    size_t numPages = readRequest_->getRequestPageIdsArraySize();
    for (size_t i = 0; i < numPages; i++)
    {
        requestedPages.insert(readRequest_->getRequestPageIds(i));
    }

    // Get the set of page ids that other clients will return
    set<FilePageId> clientCachePages;
    numPages = readRequest_->getResponseCachePageIdsArraySize();
    for (size_t i = 0; i < numPages; i++)
    {
        clientCachePages.insert(readRequest_->getResponseCachePageIds(i));
    }

    // Find the difference between the two sets
    vector<FilePageId> serverPageIds;
    set_difference(requestedPages.begin(), requestedPages.end(),
                   clientCachePages.begin(), clientCachePages.end(),
                   back_inserter(serverPageIds));

    // Set the page ids here
    readResponse->setPageIdsArraySize(serverPageIds.size());
    for (size_t i = 0; i < serverPageIds.size(); i++)
    {
        readResponse->setPageIds(i, serverPageIds[i]);
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
