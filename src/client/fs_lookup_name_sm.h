#ifndef FS_LOOKUP_NAME_SM_H
#define FS_LOOKUP_NAME_SM_H
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
#include "fs_state_machine.h"
#include "pfs_types.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIRequest;
class spfsLookupPathResponse;

/**
 * Class responsible for looking up a file system name from a client
 */
class FSLookupNameSM : public FSStateMachine
{
public:
    FSLookupNameSM(const Filename& lookupName,
                   spfsMPIRequest* mpiRequest,
                   FSClient* client);

protected:
    /** Message processing for client name lookup*/
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** @return the lookup status from the name cache */
    FSLookupStatus isNameCached();

    /** Send the request to resolve the lookup name's handle */
    void lookupHandleOnServer();

    /** Process the name resolution progress of the lookup request */
    FSLookupStatus processLookup(spfsLookupPathResponse* lookupResponse);

    /** Filename to perform lookup on */
    Filename lookupName_;

    /** The originating MPI request */
    spfsMPIRequest* mpiReq_;

    /** The filesystem client module */
    FSClient* client_;
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
