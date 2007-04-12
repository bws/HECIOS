#include <cassert>
#include <cstring>
#include <iostream>
#include "mpiio_proto_m.h"
#include "pvfs_proto_m.h"
#include "pfs_utils.h"
#include "fs_server.h"
using namespace std;

// OMNet Registration Method
Define_Module(FSServer);

int FSServer::serverNumber_ = 0;

/**
 * Initialization - Set the name and handle range
 */
void FSServer::initialize()
{
    // Set the server's name
    stringstream s;
    s << serverNumber_;
    serverName_ = "server" + s.str();
    
    // Determine this server's handle range
    range_.first = serverNumber_ * 1000;
    range_.last = range_.first + 999;

    // Increment the server number counter
    serverNumber_++;
}

/**
 * Handle MPI-IO Response messages
 */
void FSServer::handleMessage(cMessage* msg)
{
    // For now, construct the appropriate response and simply send it back
    cMessage* response;
    switch(msg->kind())
    {
        case SPFS_MPI_FILE_OPEN_REQUEST:
        {
            response = new spfsMPIFileOpenResponse(0, SPFS_MPI_FILE_OPEN_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_CLOSE_REQUEST:
        {
            response = new spfsMPIFileCloseResponse(0, SPFS_MPI_FILE_OPEN_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_DELETE_REQUEST:
        {
            response = new spfsMPIFileDeleteResponse(0, SPFS_MPI_FILE_DELETE_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_SET_SIZE_REQUEST:
        {
            response = new spfsMPIFileSetSizeResponse(0,
                                                  SPFS_MPI_FILE_SET_SIZE_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_PREALLOCATE_REQUEST:
        {
            response = new spfsMPIFilePreallocateResponse(
                0, SPFS_MPI_FILE_PREALLOCATE_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_GET_SIZE_REQUEST:
        {
            response = new spfsMPIFileGetSizeResponse(0,
                                                  SPFS_MPI_FILE_GET_SIZE_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_GET_INFO_REQUEST:
        {
            response = new spfsMPIFileGetInfoResponse(0,
                                                  SPFS_MPI_FILE_GET_INFO_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_SET_INFO_REQUEST:
        {
            response = new spfsMPIFileSetInfoResponse(0, SPFS_MPI_FILE_OPEN_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_READ_AT_REQUEST:
        {
            response = new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_READ_REQUEST:
        {
            response = new spfsMPIFileReadResponse(0, SPFS_MPI_FILE_READ_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_WRITE_AT_REQUEST:
        {
            response = new spfsMPIFileWriteAtResponse(0,
                                                  SPFS_MPI_FILE_WRITE_AT_RESPONSE);
            break;
        }
        case SPFS_MPI_FILE_WRITE_REQUEST:
        {
            response = new spfsMPIFileWriteResponse(0, SPFS_MPI_FILE_WRITE_RESPONSE);
            break;
        }
    }

    delete msg;
    send(response, "bmiOut");
    return;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
