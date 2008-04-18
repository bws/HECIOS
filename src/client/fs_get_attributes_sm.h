#ifndef FS_GET_ATTRIBUTES_SM_H
#define FS_GET_ATTRIBUTES_SM_H
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

/**
 * Class responsible for getting the attributes for a handle
 */
class FSGetAttributesSM : public FSStateMachine
{
public:
    /** Constructor for get attribute state machine */
    FSGetAttributesSM(const FSHandle& handle,
                      const FSObjectType& objectType,
                      spfsMPIRequest* mpiRequest,
                      FSClient* client);
    
    /** Constructor to locate a file's metadata attributes */
    FSGetAttributesSM(const Filename& filename,
                      spfsMPIRequest* mpiRequest,
                      FSClient* client);
    
protected:
    /** Message processing for client name lookup*/
    virtual bool updateState(cFSM& currentState, cMessage* msg);
    
private:
    /** @return true if the lookup name is cached */
    bool isAttrCached();

    /** @return true if the lookup name is cached */
    void cacheAttributes();

    /** Send the request to file's attributes */
    void getAttributes();

    /** Handle to retrieve the attributes */
    FSHandle handle_;

    /** The type of file attributes to retrieve */
    FSObjectType attributesType_;
    
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