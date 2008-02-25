#ifndef FS_CREATE_DIRECTORY_H
#define FS_CREATE_DIRECTORY_H
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
class spfsMPIDirectoryCreateRequest;

/**
 * Class responsible for creating a directory
 */
class FSCreateDirectory
{
public:
    /** Constructor */
    FSCreateDirectory(FSClient* client,
                      spfsMPIDirectoryCreateRequest* createReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

private:
    /** @return true if the parent's name is cached */
    bool isParentNameCached();

    /** @return true if the parent's attributes are cached */
    bool isParentAttrCached();

    /** Send the request to resolve the pathname up to the parent handle */
    void lookupParentOnServer();

    /** Process the name resolution progress of the lookup request */
    FSLookupStatus processLookup(spfsLookupPathResponse* lookupResponse);

    /** Send the request to create the directory's metadata object */
    void createMeta();
    
    /** Send the request to create the directory's data object */
    void createDataObject();

    /** Send the request to create a directory entry in the parent's data */
    void createDirEnt();

    /** Send the request to get the parent directory's attributes */
    void getParentAttributes();
    
    /** Add the parent directory's attributes to the cache */
    void cacheParentAttributes();
    
    /** Add the directory's name and attributes to the caches */
    void cacheNameAndAttributes();

    /** Send the final response from the client */
    void finish();
    
    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI Directory create request */
    spfsMPIDirectoryCreateRequest* createReq_;
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
