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
#include <climits>
#include <cstring>
#include <iostream>
#include "create.h"
#include "create_dir_ent.h"
#include "data_flow.h"
#include "get_attr.h"
#include "bmi_list_io_data_flow.h"
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

size_t FSServer::getDirectoryEntrySize()
{
    return 128;
}

FSServer::FSServer()
    : cSimpleModule(),
      getAttrDiskTime_("Server GetAttr Disk Time")
{
}

bool FSServer::handleIsLocal(const FSHandle& handle) const
{
    return ((handle >= range_.first) && (handle <= range_.last));
}

void FSServer::setDefaultAttrSize(size_t attrSize)
{
    defaultAttrSize_ = attrSize;
}

void FSServer::initialize()
{
    // Set the number, name, and range to invalid values
    serverNumber_ = UINT_MAX;
    serverName_ = "uninitialized";
    range_.first = UINT_MAX;
    range_.last = UINT_MAX - 1;

    // Retrieve the gate ids
    inGateId_ = gate("in")->id();
    outGateId_ = gate("out")->id();
}

void FSServer::finish()
{
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
    if (spfsRequest* req = dynamic_cast<spfsRequest*>(msg))
    {
        processRequest(req, msg);
    }
    else
    {
        cMessage* parentReq = static_cast<cMessage*>(msg->contextPointer());
        spfsRequest* origRequest =
            static_cast<spfsRequest*>(parentReq->contextPointer());
        processRequest(origRequest, msg);
        delete parentReq;
        delete msg;
    }
}

void FSServer::processRequest(spfsRequest* request, cMessage* msg)
{
    assert(0 != request);
    switch(request->kind())
    {
        case SPFS_CREATE_REQUEST:
        {
            Create create(this, static_cast<spfsCreateRequest*>(request));
            create.handleServerMessage(msg);
            break;
        }
        case SPFS_CREATE_DIR_ENT_REQUEST:
        {
            CreateDirEnt createDirEnt(
                this, static_cast<spfsCreateDirEntRequest*>(request));
            createDirEnt.handleServerMessage(msg);
            break;
        }
        case SPFS_GET_ATTR_REQUEST:
        {
            GetAttr getAttr(this, static_cast<spfsGetAttrRequest*>(request));
            getAttr.handleServerMessage(msg);
            break;
        }
        case SPFS_LOOKUP_PATH_REQUEST:
        {
            Lookup lookup(this, static_cast<spfsLookupPathRequest*>(request));
            lookup.handleServerMessage(msg);
            break;
        }
        case SPFS_READ_REQUEST:
        {
            Read read(this, static_cast<spfsReadRequest*>(request));
            read.handleServerMessage(msg);
            break;
        }
        case SPFS_SET_ATTR_REQUEST:
        {
            SetAttr setAttr(this, static_cast<spfsSetAttrRequest*>(request));
            setAttr.handleServerMessage(msg);
            break;
        }
        case SPFS_WRITE_REQUEST:
        {
            Write write(this, static_cast<spfsWriteRequest*>(request));
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

void FSServer::send(cMessage* msg)
{
    cSimpleModule::send(msg, outGateId_);
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
