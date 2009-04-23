#ifndef FS_CREATE_DIRECTORY_SM_H_
#define FS_CREATE_DIRECTORY_SM_H_

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
#include "filename.h"
#include "fs_state_machine.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIRequest;

/**
 * Class responsible for creating a directory
 */
class FSCreateDirectorySM : public FSStateMachine
{
public:
    /** Construct the directory creation state machine */
    FSCreateDirectorySM(const Filename& dirname,
                        spfsMPIRequest* mpiReq,
                        FSClient* client);

protected:
    /** Message processing for serial creates */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send the metadata creation message */
    void createMeta();

    /** Send the data object creation message */
    void createDataObject();

    /** Send the metadata creation message */
    void countDataCreationResponse();

    /** @return true if all the data objects have been create */
    bool isDataCreationComplete();

    /** Send the message to set the metadata */
    void writeAttributes();

    /** Send the directory entry creation message */
    void createDirEnt();

    /** Insert the cache entries for this directory */
    void updateClientCache();

    /** The name of the file to create */
    Filename createDirName_;

    /** The originating MPI request */
    spfsMPIRequest* mpiReq_;

    /** The filesystem client module */
    FSClient* client_;
};

#endif /* FS_CREATE_DIRECTORY_SM_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
