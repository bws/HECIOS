#ifndef COLLECTIVE_CREATE_H
#define COLLECTIVE_CREATE_H
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
#include "pfs_types.h"
class cArray;
class cMessage;
class spfsCollectiveCreateRequest;
class FSServer;

/**
 * State machine for performing collective data object creation processing
 */
class CollectiveCreate
{
public:
    /** Constructor */
    CollectiveCreate(FSServer* module, spfsCollectiveCreateRequest* createReq);

    /**
     * Handle message as part of the creation process
     */
    void handleServerMessage(cMessage* msg);

protected:
    /** Send the collective metadata creation message */
    void sendMeta();
    
    /** Send the file creation message to the OS */
    void enterCreate();

    /** Send a message to set the attributes */
    void setAttributes();
    
    /** Send the file write message to the OS */
    void createDirEnt();
    
    /** Send the child collective create requests for this node */
    void sendCollectiveRequests();
    
    /**
     * Process responses from servers and OS
     *
     * @return true if no more outstanding responses exist
     */
    bool processResponse(cMessage* response);
    
    /** Send the final response to the client */
    void enterFinish();
    
private:
    /** Create a child collective request */
    spfsCollectiveCreateRequest* createChildCollectiveRequest(
        int idx, int numHandles) const;

    /** The parent module */
    FSServer* module_;
    
    /** The originating collective create request */
    spfsCollectiveCreateRequest* createReq_;
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
