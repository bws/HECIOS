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
#include <iostream>
#include "mpi_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class FSClient : public cSimpleModule
{
public:
    /** Constructor */
    FSClient() : cSimpleModule() {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize() {};

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

};

// OMNet Registriation Method
//Define_Module(FSClient);

/**
 * Handle MPI-IO Response messages
 */
void FSClient::handleMessage(cMessage* msg)
{
    switch(msg->kind())
    {
        case SPFS_MPI_FILE_OPEN_REQUEST:
        case SPFS_MPI_FILE_CLOSE_REQUEST:
        case SPFS_MPI_FILE_DELETE_REQUEST:
        case SPFS_MPI_FILE_SET_SIZE_REQUEST:
        case SPFS_MPI_FILE_PREALLOCATE_REQUEST:
        case SPFS_MPI_FILE_GET_SIZE_REQUEST:
        case SPFS_MPI_FILE_GET_INFO_REQUEST:
        case SPFS_MPI_FILE_SET_INFO_REQUEST:
        case SPFS_MPI_FILE_READ_AT_REQUEST:
        case SPFS_MPI_FILE_READ_REQUEST:
        case SPFS_MPI_FILE_WRITE_AT_REQUEST:
        case SPFS_MPI_FILE_WRITE_REQUEST:
        {
            cerr << "FSClient::handleMessage forwarding implementation "
                 << "for request: "<< msg->kind() << endl;
            send(msg, "netOut");
            break;
        }
        case SPFS_MPI_FILE_OPEN_RESPONSE:
        case SPFS_MPI_FILE_CLOSE_RESPONSE:
        case SPFS_MPI_FILE_DELETE_RESPONSE:
        case SPFS_MPI_FILE_SET_SIZE_RESPONSE:
        case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
        case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
        case SPFS_MPI_FILE_GET_INFO_RESPONSE:
        case SPFS_MPI_FILE_SET_INFO_RESPONSE:
        case SPFS_MPI_FILE_READ_AT_RESPONSE:
        case SPFS_MPI_FILE_READ_RESPONSE:
        case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
        case SPFS_MPI_FILE_WRITE_RESPONSE:
        {
            cerr << "FSClient::handleMessage forwarding implementation "
                 << "for response: "<< msg->kind() << endl;
            send(msg, "appOut");
            break;
        }
        default:
            cerr << "FSClient::handleMessage not yet implemented "
                 << "for kind: "<< msg->kind() << endl;
            break;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
