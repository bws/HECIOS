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

#include <cassert>
#include <cstring>
#include <iostream>
#include "create.h"
#include "get_attr.h"
#include "lookup.h"
#include "read.h"
#include "set_attr.h"
#include "write.h"
#include "pvfs_proto_m.h"
#include "pfs_utils.h"
#include "fs_server.h"
using namespace std;

// OMNet Registration Method
Define_Module(FSServer);

size_t FSServer::defaultAttrSize_ = 0;

size_t FSServer::getDefaultAttrSize()
{
    return defaultAttrSize_;
}

void FSServer::setDefaultAttrSize(size_t attrSize)
{
    defaultAttrSize_ = attrSize;
}

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

    // Retrieve the gate ids
    netInGateId_ = gate("netIn")->id();
    netOutGateId_ = gate("netOut")->id();
    storageInGateId_ = gate("storageIn")->id();
    storageOutGateId_ = gate("storageOut")->id();
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


void FSServer::handleMessage(cMessage* msg)
{
    // If the message is a new client request, process it directly
    // Otherwise its a response, extract the originating request
    // and then process the response
    if (msg->arrivalGateId() == netInGateId_)
    {
        processMessage(dynamic_cast<spfsRequest*>(msg), msg);
    }
    else
    {
        cMessage* parentReq = static_cast<cMessage*>(msg->contextPointer());
        spfsRequest* origRequest =
            static_cast<spfsRequest*>(parentReq->contextPointer());
        processMessage(origRequest, msg);
    }
}

void FSServer::processMessage(spfsRequest* request, cMessage* msg)
{
    switch(request->kind())
    {
        case SPFS_CREATE_REQUEST:
        {
            Create create(this, dynamic_cast<spfsCreateRequest*>(request));
            create.handleServerMessage(msg);
            break;
        }
        case SPFS_GET_ATTR_REQUEST:
        {
            GetAttr getAttr(this, dynamic_cast<spfsGetAttrRequest*>(request));
            getAttr.handleServerMessage(msg);
            break;
        }
        case SPFS_LOOKUP_PATH_REQUEST:
        {
            Lookup lookup(this, dynamic_cast<spfsLookupPathRequest*>(request));
            lookup.handleServerMessage(msg);
            break;
        }
        case SPFS_READ_REQUEST:
        {
            Read read(this, dynamic_cast<spfsReadRequest*>(request));
            read.handleServerMessage(msg);
            break;
        }
        case SPFS_SET_ATTR_REQUEST:
        {
            SetAttr setAttr(this, dynamic_cast<spfsSetAttrRequest*>(request));
            setAttr.handleServerMessage(msg);
            break;
        }
        case SPFS_WRITE_REQUEST:
        {
            Write write(this, dynamic_cast<spfsWriteRequest*>(request));
            write.handleServerMessage(msg);
            break;
        }
        default:
        {
            cerr << "FSServer Error: Unknown message kind:" << request->kind()
                 << endl
                 << "!!!!!!!!! ------------------------ !!!!!!!!!!!!!" << endl
                 << "ERROR: Server unable to construct response" << endl
                 << "!!!!!!!!! ------------------------ !!!!!!!!!!!!!" << endl;
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
