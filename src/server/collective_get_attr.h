#ifndef COLLECTIVE_GET_ATTR_H
#define COLLECTIVE_GET_ATTR_H
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
class spfsCollectiveGetAttrRequest;
class FSServer;

/**
 * State machine for performing collective get attribute processing
 */
class CollectiveGetAttr
{
public:
    /** Constructor */
    CollectiveGetAttr(FSServer* module, spfsCollectiveGetAttrRequest* getAttrReq);

    /**
     * Handle message as part of the creation process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the file creation message to the OS
     */
    void getLocalAttributes();

    /** Send the child collective create requests for this node */
    void sendCollectiveRequests();
    
    /**
     *Process responses from servers and OS
     *
     * @return true if no more outstanding responses exist
     */
    bool processResponse(cMessage* response);
    
    /**
     * Send the final response to the client
     */
    void enterFinish();
    
private:
    /** Create a child collective request */
    spfsCollectiveGetAttrRequest* createChildCollectiveRequest(
        int idx, int numHandles) const;

    /** The parent module */
    FSServer* module_;
    
    /** The originating collective create request */
    spfsCollectiveGetAttrRequest* getAttrReq_;
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
