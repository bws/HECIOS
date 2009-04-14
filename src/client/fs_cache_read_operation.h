#ifndef FS_READ_OPERATION_H_
#define FS_READ_OPERATION_H_
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
#include "fs_operation.h"
class cFSM;
class cMessage;
class FSClient;
class spfsCacheReadRequest;
class spfsCacheReadExclusiveRequest;
class spfsCacheReadSharedRequest;

/** Class responsible for performing client-side file reads */
class FSCacheReadOperation : public FSOperation
{
public:
    /** Construct operation for read exclusive */
    FSCacheReadOperation(FSClient* client, spfsCacheReadExclusiveRequest* readRequest);

    /** Construct operation for read shared */
    FSCacheReadOperation(FSClient* client, spfsCacheReadSharedRequest* readRequest);

    /** Destructor */
    ~FSCacheReadOperation();

protected:
    /** Register state machines to perform the read */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** The file system client module */
    FSClient* client_;

    /** The originating cache read request */
    spfsCacheReadRequest* readRequest_;

    /** Indicate whether this is a read exclusive or read shared */
    bool isExclusive_;
};

#endif /* FS_READ_OPERATION_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
