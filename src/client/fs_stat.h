#ifndef FS_STAT_H
#define FS_STAT_H
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
class cMessage;
class FSClient;
class spfsLookupPathResponse;
class spfsMPIFileStatRequest;

/**
 * Class responsible for performing client-side file stat operations
 */
class FSStat
{
public:

    /** Constructor */
    FSStat(FSClient* client, spfsMPIFileStatRequest* statReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:
    /** Send message to retrieve the metadata attributes */
    void getMetaDataAttributes();

    /** Send messages to retrieve the data attributes */
    void getDataAttributes();

    /**
     * @return true if all of the data attribute responses have been received
     */
    bool processResponse(cMessage* getAttrResponse);
    
    /** Send final client response */
    void finish();
    
private:
    /** */
    bool isParentNameCached();
    bool isParentAttrCached();
    
    /** */
    void lookupParentOnServer();
    void getParentAttributes();
    void cacheParentAttributes();
    void lookupNameOnServer();
    FSLookupStatus processLookup(spfsLookupPathResponse* lookupResponse);
    void writeAttributes();
    
    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI Stat request */
    spfsMPIFileStatRequest* statReq_;
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
