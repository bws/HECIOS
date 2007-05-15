#include <cassert>
#include <cstring>
#include <iostream>
#include "create.h"
#include "lookup.h"
#include "mpiio_proto_m.h"
#include "pvfs_proto_m.h"
#include "pfs_utils.h"
#include "fs_server.h"
using namespace std;

// OMNet Registration Method
Define_Module(FSServer);

/**
 * Initialization - Set the name and handle range
 */
void FSServer::initialize()
{
    // Determine this server's handle range
    range_.first = serverNumber_ * 1000;
    range_.last = range_.first + 999;

    // Set the number and name to invalid values
    serverNumber_ = -1;
    serverName_ = "uninitialized";
}

void FSServer::setNumber(size_t number)
{
    // Set the server number
    serverNumber_ = number;

    // Set the server's name
    stringstream s;
    s << serverNumber_;
    serverName_ = "server" + s.str();
    
}
/**
 * Handle MPI-IO Response messages
 */
void FSServer::handleMessage(cMessage* msg)
{
    // For now, construct the appropriate response and simply send it back
    switch(msg->kind())
    {
        case SPFS_CREATE_REQUEST:
        {
            cMessage* createResponse;
            Create create(static_cast<spfsCreateRequest*>(msg));
            createResponse = create.handleServerMessage(msg);
            send(createResponse, "netOut");
            break;
        }
        case SPFS_LOOKUP_PATH_REQUEST:
        {
            cMessage* lookupResponse;
            Lookup lookup(static_cast<spfsLookupPathRequest*>(msg));
            lookupResponse = lookup.handleServerMessage(msg);
            send(lookupResponse, "netOut");
            break;
        }
        default:
        {
            cerr << "Error: Unknown server message type" << endl;
        }
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
