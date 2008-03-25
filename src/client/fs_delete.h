#ifndef FS_DELETE_H
#define FS_DELETE_H
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
#include "pfs_types.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIDirectoryRemoveRequest;
class spfsMPIFileDeleteRequest;
class spfsMPIRequest;
class spfsRemoveResponse;
class spfsRemoveDirEntResponse;
class spfsGetAttrResponse;
class spfsLookupPathResponse;
class spfsSetAttrResponse;

/**
 * Class responsible for deletinging a file or directory
 */
class FSDelete
{
public:
    /** Construct FS Delete processor with collective optimization settable */
    FSDelete(FSClient* client,
             spfsMPIFileDeleteRequest* deleteFileReq,
             bool useCollectiveCommunication);
    
    /** Construct FS Delete processor with collective optimization settable */
    FSDelete(FSClient* client,
             spfsMPIDirectoryRemoveRequest* deleteDirReq,
             bool useCollectiveCommunication);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:
    /** Message processing for opens with serial creates */
    void serialMessageProcessor(cMessage* msg);

    /** Message processing for Opens with collective creates */
    void collectiveMessageProcessor(cMessage* msg);
    
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
    bool checkFileCreateFlags();
    
    /** Create metadata object */
    void createMeta();

    /** Create data objects */
    void createDataObjects();

    /** @return true when all the data remove responses have been received */
    bool countDataRemoveResponse();

    /** Send message to parent server to remove directory entry */
    void removeDirEnt();

    /** Send message to meta server to remove attributes */
    void removeMeta();

    /** Send message to data servers to remove data objects */
    void removeData();

    /** Send final response */
    void finish();

    /** Send the collective remove message */
    void collectiveRemove();
    
private:
    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI request */
    spfsMPIRequest* clientReq_;

    /** Use server to server based collectives to create file */
    bool useCollectiveCommunication_;

    /** FS Name to delete */
    Filename deleteName_;
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
