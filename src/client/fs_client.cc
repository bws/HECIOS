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
#include "fs_client.h"
#include <iostream>
#include "fs_close.h"
#include "fs_create_directory.h"
#include "fs_open.h"
#include "fs_read.h"
#include "fs_write.h"
#include "pfs_types.h"
#include "pvfs_proto_m.h"
#include "mpi_proto_m.h"
using namespace std;

// Define FSClient module for this class
Define_Module(FSClient);

FSClient::FSClient()
    : createDirEntDelay("Client CrDirEnt Roundtrip Delay"),
      createObjectDelay("Client CreateObject Roundtrip Delay"),
      getAttrDelay("Client GetAttr Roundtrip Delay")
{
}

void FSClient::initialize()
{
    appInGateId_ = findGate("appIn");
    appOutGateId_ = findGate("appOut");
    netInGateId_ = findGate("netIn");
    netOutGateId_ = findGate("netOut");
}

void FSClient::finish()
{
}

void FSClient::handleMessage(cMessage *msg)
{
    if (msg->arrivalGateId() == appInGateId_)
    {
        processMessage(msg, msg);
    }
    else
    {
        // Collect data about the response
        collectServerResponseData(msg);

        // Extract the originating client request
        cMessage* parentReq = static_cast<cMessage*>(msg->contextPointer());
        cMessage* originalClientRequest =
            static_cast<cMessage*>(parentReq->contextPointer());
        processMessage(originalClientRequest, msg);

        // Only cleanup the server request if the autoCleanup
        // flag is set.  This is necessary because some requests have
        // multiple responses, and the individual state machine will
        // have to handle deleting the request at the appropriate time
        spfsRequest* serverReq = dynamic_cast<spfsRequest*>(parentReq);
        assert(0 != serverReq);
        if (true == serverReq->getAutoCleanup())
        {
            delete serverReq;
        }

        // Cleanup the response
        delete msg;
    }
}

void FSClient::processMessage(cMessage* request, cMessage* msg)
{
    switch(request->kind())
    {
        case SPFS_MPI_DIRECTORY_CREATE_REQUEST:
        {
            FSCreateDirectory dirCreate(
                this, static_cast<spfsMPIDirectoryCreateRequest*>(request));
            dirCreate.handleMessage(msg);
            break;
        }
        case SPFS_MPI_FILE_OPEN_REQUEST:
        {
            FSOpen open(this,
                        static_cast<spfsMPIFileOpenRequest*>(request));
            open.handleMessage(msg);
            break;
        }
        case SPFS_MPI_FILE_CLOSE_REQUEST :
        {
            FSClose close(this,
                          static_cast<spfsMPIFileCloseRequest*>(request));
            close.handleMessage(msg);
            break;
        }
        case SPFS_MPI_FILE_READ_AT_REQUEST:
        {
            FSRead read(this,
                        static_cast<spfsMPIFileReadAtRequest*>(request));
            read.handleMessage(msg);
            break;
        }
        case SPFS_MPI_FILE_WRITE_AT_REQUEST:
        {
            FSWrite write(this,
                          static_cast<spfsMPIFileWriteAtRequest*>(request));
            write.handleMessage(msg);
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
        case SPFS_MPI_FILE_DELETE_REQUEST:
        case SPFS_MPI_FILE_SET_SIZE_REQUEST :
        case SPFS_MPI_FILE_PREALLOCATE_REQUEST :
        case SPFS_MPI_FILE_GET_SIZE_REQUEST :
        {
            cerr << "ERROR FSClient: Unsupported client request type: "
                 << request->kind()
                 << endl;
            break;
        }
        default:
        {
            cerr << "FSClient: Unknown Message: " << request->kind()
                 << " " << request->info() << endl;
            break;
        }
    }
}

void FSClient::collectServerResponseData(cMessage* serverResponse)
{
    cMessage* parentRequest =
        static_cast<cMessage*>(serverResponse->contextPointer());

    // Determine the request response roundtrip time
    simtime_t reqSendTime = parentRequest->creationTime();
    simtime_t respArriveTime = simTime();
    simtime_t delay = respArriveTime - reqSendTime;
    
    switch(serverResponse->kind())
    {
        case SPFS_CREATE_DIR_ENT_RESPONSE:
        {
            createDirEntDelay.record(delay);
            break;
        }
        case SPFS_CREATE_RESPONSE:
        {
            createObjectDelay.record(delay);
            break;
        }
        case SPFS_GET_ATTR_RESPONSE:
        {
            getAttrDelay.record(delay);
            break;
        }
        default:
        {
            cerr << "Unable to collect data for message: "
                 << serverResponse->kind() << endl;
        }
    }
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
