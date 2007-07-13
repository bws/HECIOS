#include <cassert>
#include <cstring>
#include <iostream>
#include "create.h"
#include "get_attr.h"
#include "lookup.h"
#include "read.h"
#include "write.h"
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
    // Set the number, name, and range to invalid values
    serverNumber_ = -1;
    serverName_ = "uninitialized";
    range_.first = -1;
    range_.last = -1;
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
    cMessage* response = 0;
    
    switch(msg->kind())
    {
        case SPFS_CREATE_REQUEST:
        {
            Create create(static_cast<spfsCreateRequest*>(msg));
            response = create.handleServerMessage(msg);
            break;
        }
        case SPFS_LOOKUP_PATH_REQUEST:
        {
            Lookup lookup(static_cast<spfsLookupPathRequest*>(msg));
            response = lookup.handleServerMessage(msg);
            break;
        }
        case SPFS_GET_ATTR_REQUEST:
        {
            GetAttr getAttr(static_cast<spfsGetAttrRequest*>(msg));
            response = getAttr.handleServerMessage(msg);
            break;
        }
        case SPFS_READ_REQUEST:
        {
            Read read(static_cast<spfsReadRequest*>(msg));
            response = read.handleServerMessage(msg);
            break;
        }
        case SPFS_WRITE_REQUEST:
        {
            Write write(static_cast<spfsWriteRequest*>(msg));
            response = write.handleServerMessage(msg);
            break;
        }
        default:
        {
            cerr << "Error: Unknown server message type" << endl;
        }
    }

    // Send server's response
    if (spfsResponse* resp = dynamic_cast<spfsResponse*>(response))
    {
        cerr << "Request message: " << msg->info() << " " << msg->kind() << endl;
        spfsRequest* req = dynamic_cast<spfsRequest*>(msg);
        assert(0 != req);
        resp->setContextPointer(req);
        resp->setSocketId(req->getSocketId());
        send(resp, "netOut");
    }
    else
    {
        cerr << "!!!!!!!!! ------------------------ !!!!!!!!!!!!!!" << endl;
        cerr << "ERROR: Server unable to construct proper response!" << endl;
        cerr << "!!!!!!!!! ------------------------ !!!!!!!!!!!!!!" << endl;
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
