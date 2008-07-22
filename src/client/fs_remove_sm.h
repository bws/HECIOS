#ifndef FS_REMOVE_SM_H
#define FS_REMOVE_SM_H
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
class cFSM;
class cMessage;
class FSClient;
class spfsMPIRequest;

/**
 * Class responsible for removing a file
 */
class FSRemoveSM : public FSStateMachine
{
public:
    /** Construct the file remove state machine */
    FSRemoveSM(const Filename& filename,
               spfsMPIRequest* mpiReq,
               FSClient* client);

protected:
    /** Message processing for removes */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send the directory entry remove message */
    void removeDirEnt();

    /** Send the metadata remove message */
    void removeMeta();

    /** Send the data object remove messages */
    void removeDataObjects();

    /** @return true if all the data objects have been create */
    bool countDataRemoveResponse();

    /** Remove entries from the client cache */
    void removeNameAndAttributesFromCache();

    /** The name of the file to remove */
    Filename removeName_;

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
