#ifndef FS_DELETE_OPERATION_H
#define FS_DELETE_OPERATION_H
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
#include "filename.h"
#include "fs_client_operation.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIFileDeleteRequest;
class spfsMPIDirectoryRemoveRequest;

/**
 * Class responsible for deleting a file
 */
class FSDeleteOperation : public FSClientOperation
{
public:
    /** Construct FS Delete processor for a file */
    FSDeleteOperation(FSClient* client,
                      spfsMPIFileDeleteRequest* deleteReq,
                      bool useCollectiveCommunication);
    
    /** Construct FS Delete processor for a file */
    FSDeleteOperation(FSClient* client,
                      spfsMPIDirectoryRemoveRequest* rmDirReq,
                      bool useCollectiveCommunication);
    
protected:
    /** Register the state machines to perform a file deletion */
    virtual void registerStateMachines();
    
    /** Send final response */
    virtual void sendFinalResponse();

private:
    
    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI delete/remove request */
    spfsMPIRequest* deleteReq_;
    
    /** Use server to server based collectives to delete file */
    bool useCollectiveCommunication_;

    /** The name to delete */
    Filename deleteName_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
