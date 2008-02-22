#ifndef FS_UPDATE_TIME_H
#define FS_UPDATE_TIME_H
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
#include "basic_types.h"
#include "pvfs_proto_m.h"
class cMessage;
class FSClient;
class spfsMPIFileUpdateTimeRequest;

/**
 * Class responsible for performing client-side file time updates
 */
class FSUpdateTime
{
public:

    /** Constructor */
    FSUpdateTime(FSClient* client, spfsMPIFileUpdateTimeRequest* utimeReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:
    /** Send message setting the utime attribute */
    void updateTime();

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
    spfsLookupStatus processLookup(spfsLookupPathResponse* lookupResponse);
    void writeAttributes();
    
    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI update time request */
    spfsMPIFileUpdateTimeRequest* utimeReq_;
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
