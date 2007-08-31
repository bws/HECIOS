//
// This file is part of Hecios
//
// Copyright (C) 2007 Walt Ligon
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
#include "fs_module.h"
#include <iostream>
#include "fs_close.h"
#include "fs_open.h"
#include "fs_read.h"
#include "fs_write.h"
#include "pfs_types.h"
#include "pvfs_proto_m.h"
#include "mpi_proto_m.h"
using namespace std;

// local function decls
static void fsUnknownMessage(cMessage *req, cMessage *resp, fsModule *client);
static void fsProcessMessage(cMessage *req, cMessage *resp, fsModule *client);
static void fsProcessTimer( cMessage *msg );


Define_Module(fsModule);

void fsModule::initialize()
{
    fsMpiOut = findGate("appOut");
    fsMpiIn = findGate("appIn");
    fsNetOut = findGate("netOut");
    fsNetIn = findGate("netIn");
}

void fsModule::finish()
{
    // need to free anything here?
}

void fsModule::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        fsProcessTimer(msg);
    else
        fsProcessMessage(msg, NULL, this);
}

void fsProcessMessage(cMessage *req, cMessage *resp, fsModule *client)
{
    // Call message specific hander
    switch(req->kind())
    {
        case SPFS_MPI_FILE_OPEN_REQUEST:
        {
            FSOpen open(client, static_cast<spfsMPIFileOpenRequest*>(req));
            if (resp)
                open.handleMessage(resp);
            else
                open.handleMessage(req);
            break;
        }
        case SPFS_MPI_FILE_CLOSE_REQUEST :
        {
            FSClose close(client, static_cast<spfsMPIFileCloseRequest*>(req));
            if (resp)
                close.handleMessage(resp);
            else
                close.handleMessage(req);
            break;
        }
        case SPFS_MPI_FILE_READ_REQUEST:
        {
            cerr << "ERROR: Illegal read message!" << endl;
            break;
        }
        case SPFS_MPI_FILE_WRITE_REQUEST:
        {
            cerr << "ERROR: Illegal write message!" << endl;
            break;
        }
        case SPFS_MPI_FILE_READ_AT_REQUEST:
        {
            FSRead read(client, static_cast<spfsMPIFileReadAtRequest*>(req));
            if (resp)
                read.handleMessage(resp);
            else
                read.handleMessage(req);
            break;
        }
        case SPFS_MPI_FILE_WRITE_AT_REQUEST:
        {
            FSWrite write(client, static_cast<spfsMPIFileWriteAtRequest*>(req));
            if (resp)
                write.handleMessage(resp);
            else
                write.handleMessage(req);
            break;
        }
        case SPFS_MPI_FILE_DELETE_REQUEST:
        case SPFS_MPI_FILE_SET_SIZE_REQUEST :
        case SPFS_MPI_FILE_PREALLOCATE_REQUEST :
        case SPFS_MPI_FILE_GET_SIZE_REQUEST :
        {
            cerr << "ERROR fsModule: Unsupported client request type: "
                 << req->kind()
                 << endl;
            break;
        }

        // Server response messages
        case SPFS_CREATE_RESPONSE :
        case SPFS_LOOKUP_PATH_RESPONSE :
        case SPFS_GET_ATTR_RESPONSE :
        case SPFS_SET_ATTR_RESPONSE :
        case SPFS_READ_RESPONSE:
        case SPFS_WRITE_RESPONSE :
        case SPFS_REMOVE_RESPONSE :
        case SPFS_CREATE_DIR_ENT_RESPONSE :
        case SPFS_REMOVE_DIR_ENT_RESPONSE :
        case SPFS_CHANGE_DIR_ENT_RESPONSE :
        case SPFS_TRUNCATE_RESPONSE :
        case SPFS_MAKE_DIR_RESPONSE :
        case SPFS_READ_DIR_RESPONSE :
        case SPFS_WRITE_COMPLETION_RESPONSE :
        case SPFS_FLUSH_RESPONSE :
        case SPFS_STAT_RESPONSE :
        case SPFS_LIST_ATTR_RESPONSE :
        {
            cMessage* spfsRequest = (cMessage*)req->contextPointer();
            cMessage* mpiRequest = (cMessage*)spfsRequest->contextPointer();
            fsProcessMessage(mpiRequest, req, client);

            // Cleanup the server request
            delete spfsRequest;
            spfsRequest = 0;
            break;
        }   
        default:
        {
            fsUnknownMessage(req, resp, client);
            break;
        }
    }
}

void fsUnknownMessage(cMessage *req, cMessage *resp, fsModule *client)
{
    cerr << "FsModule: Unknown Message: " << req->kind()
         << " " << req->info() << endl;
}

// timers from self
void fsProcessTimer( cMessage *msg )
{
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
