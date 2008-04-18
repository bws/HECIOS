#ifndef FS_CLOSE_OPERATION_H
#define FS_CLOSE_OPERATION_H
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
#include "fs_client_operation.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIFileCloseRequest;

/**
 * Class responsible for closing a file
 */
class FSCloseOperation : public FSClientOperation
{
public:
    /** Construct FSClose processor */
    FSCloseOperation(FSClient* client, spfsMPIFileCloseRequest* closeReq);
    
protected:
    /** No op */
    virtual void registerStateMachines();
    
    /** Send final response */
    virtual void sendFinalResponse();

private:
    
    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI Close request */
    spfsMPIFileCloseRequest* closeReq_;
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