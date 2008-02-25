#ifndef FS_OPEN_H
#define FS_OPEN_H
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
class cFSM;
class cMessage;
class FSClient;
class spfsMPIFileOpenRequest;
class spfsCreateResponse;
class spfsCreateDirEntResponse;
class spfsGetAttrResponse;
class spfsLookupPathResponse;
class spfsSetAttrResponse;

/**
 * Class responsible for opening a file
 */
class FSOpen
{
public:

    /** Constructor */
    FSOpen(FSClient* client, spfsMPIFileOpenRequest* openReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:
    /** @return true if the parent's name is cached */
    bool isParentNameCached();

    /** @return true if the parent's attributes are cached */
    bool isParentAttrCached();

    /** Send the request to resolve the pathname up to the parent handle */
    void lookupParentOnServer();

    /** Send the request to get the parent directory's attributes */
    void getParentAttributes();
    
    /** Add the parent directory's attributes to the cache */
    void cacheParentAttributes();
    
    /** Send the request to resolve the file name to a handle */
    void lookupNameOnServer();

    /** Process the name resolution progress of the lookup request */
    FSLookupStatus processLookup(spfsLookupPathResponse* lookupResponse);

    /** @return true if the file does not exist and should be created,
        else it returns false */
    bool checkFileCreateFlags(const FSLookupStatus& status);
    
    /** Create metadata object */
    void createMeta();

    /** Create data objects */
    void createDataObjects();

    /** Count the incoming data object creation responses */
    void countDataCreationResponse();

    /** @return true when all the data objects have been created */
    bool isDataCreationComplete();

    /** Send message to meta server to write attributes */
    void writeAttributes();

    /** Create directory entry */
    void createDirEnt();

    /** Read attributes */
    void readAttributes();

    /** Add the lookup file's attributes to cache */
    void addAttributesToCache();

    /** Finalize operation */
    void finish();
    
private:

    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI Open request */
    spfsMPIFileOpenRequest* openReq_;
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
