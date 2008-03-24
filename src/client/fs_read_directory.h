#ifndef FS_READ_DIRECTORY_H
#define FS_READ_DIRECTORY_H
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
class spfsMPIDirectoryReadRequest;

/**
 * Class responsible for reading a directory
 */
class FSReadDirectory
{
public:
    /** Constructor */
    FSReadDirectory(FSClient* client, spfsMPIDirectoryReadRequest* readReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

private:
    /** Send the request to read the directory's attributes */
    void getAttributes();
    
    /** Send the request to read the directory's entries */
    void addAttributesToCache();
    
    /** Send the request to read the directory's entries */
    void readDirEnt();
    
    /** Send the final response from the client */
    void finish();
    
    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI Directory read request */
    spfsMPIDirectoryReadRequest* readReq_;
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
