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
simtime_t FSServer::createDFileProcessingDelay_ = 0.0;
simtime_t FSServer::createDirectoryProcessingDelay_ = 0.0;
simtime_t FSServer::createMetadataProcessingDelay_ = 0.0;
simtime_t FSServer::createDirEntProcessingDelay_ = 0.0;
simtime_t FSServer::getAttrProcessingDelay_ = 0.0;
simtime_t FSServer::lookupPathProcessingDelay_ = 0.0;
simtime_t FSServer::setAttrProcessingDelay_ = 0.0;

size_t FSServer::getDefaultAttrSize()
{
    return defaultAttrSize_;
}

size_t FSServer::getDirectoryEntrySize()
{
    return 128;
}

void FSServer::setCreateDFileProcessingDelay(simtime_t createDFileDelay)
{
    createDFileProcessingDelay_ = createDFileDelay;
}

simtime_t FSServer::createDFileProcessingDelay()
{
    return createDFileProcessingDelay_;
}

void FSServer::setCreateDirectoryProcessingDelay(simtime_t createDirDelay)
{
    createDirectoryProcessingDelay_ = createDirDelay;
}

simtime_t FSServer::createDirectoryProcessingDelay()
{
    return createDirectoryProcessingDelay_;
}

void FSServer::setCreateMetadataProcessingDelay(simtime_t createMetaDelay)
{
    createMetadataProcessingDelay_ = createMetaDelay;
}

simtime_t FSServer::createMetadataProcessingDelay()
{
    return createMetadataProcessingDelay_;
}

void FSServer::setCreateDirEntProcessingDelay(simtime_t createDirEntDelay)
{
    createDirEntProcessingDelay_ = createDirEntDelay;
}

simtime_t FSServer::createDirEntProcessingDelay()
{
    return createDirEntProcessingDelay_;
}

void FSServer::setGetAttrProcessingDelay(simtime_t getAttrDelay)
{
    getAttrProcessingDelay_ = getAttrDelay;
}

simtime_t FSServer::getAttrProcessingDelay()
{
    return getAttrProcessingDelay_;
}

void FSServer::setLookupPathProcessingDelay(simtime_t lookupPathDelay)
{
    lookupPathProcessingDelay_ = lookupPathDelay;
}

simtime_t FSServer::lookupPathProcessingDelay()
{
    return lookupPathProcessingDelay_;
}

void FSServer::setSetAttrProcessingDelay(simtime_t setAttrDelay)
{
    setAttrProcessingDelay_ = setAttrDelay;
}

simtime_t FSServer::setAttrProcessingDelay()
{
    return setAttrProcessingDelay_;
}

FSServer::FSServer()
    : cSimpleModule(),
      createDirEntDiskDelay_("SPFS Create DirEnt Disk Delay"),
      createObjectDiskDelay_("SPFS Create Object Disk Delay"),
      getAttrDiskDelay_("SPFS GetAttr Disk Delay"),
      lookupDiskDelay_("SPFS Lookup Disk Delay"),
      setAttrDiskDelay_("SPFS SetAttr Disk Delay")
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

    // Initialize scalar data
    numCreateDirEnts_ = 0;
    numCreateObjects_ = 0;
    numGetAttrs_ = 0;
    numLookups_ = 0;
    numReads_ = 0;
    numSetAttrs_ = 0;
    numWrites_ = 0;
}

void FSServer::finish()
{
    recordScalar("SPFS Server CrDirEnts", numCreateDirEnts_);
    recordScalar("SPFS Server CreateObjects", numCreateObjects_);
    recordScalar("SPFS Server GetAttrs", numGetAttrs_);
    recordScalar("SPFS Server Lookups", numLookups_);
    recordScalar("SPFS Server Reads", numReads_);
    recordScalar("SPFS Server SetAttrs", numSetAttrs_);
    recordScalar("SPFS Server Writes", numWrites_);
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

void FSServer::sendDelayed(cMessage* msg, simtime_t delay)
{
    cSimpleModule::sendDelayed(msg, delay, outGateId_);
}

void FSServer::recordCreateDirEnt()
{
    numCreateDirEnts_++;
}

void FSServer::recordCreateObject()
{
    numCreateObjects_++;
}

void FSServer::recordGetAttr()
{
    numGetAttrs_++;
}

void FSServer::recordLookup()
{
    numLookups_++;
}

void FSServer::recordRead()
{
    numReads_++;
}

void FSServer::recordSetAttr()
{
    numSetAttrs_++;
}

void FSServer::recordWrite()
{
    numWrites_++;
}

void FSServer::recordCreateDirEntDiskDelay(cMessage* fileWriteResponse)
{
    createDirEntDiskDelay_.record(getRoundTripDelay(fileWriteResponse));
}

void FSServer::recordCreateObjectDiskDelay(cMessage* fileOpenResponse)
{
    createObjectDiskDelay_.record(getRoundTripDelay(fileOpenResponse));
}

void FSServer::recordGetAttrDiskDelay(cMessage* fileReadResponse)
{
    getAttrDiskDelay_.record(getRoundTripDelay(fileReadResponse));
}

void FSServer::recordLookupDiskDelay(cMessage* fileReadResponse)
{
    lookupDiskDelay_.record(getRoundTripDelay(fileReadResponse));
}

void FSServer::recordSetAttrDiskDelay(cMessage* fileWriteResponse)
{
    setAttrDiskDelay_.record(getRoundTripDelay(fileWriteResponse));
}

simtime_t FSServer::getRoundTripDelay(cMessage* response) const
{
    // Get the originating request
    cMessage* request = static_cast<cMessage*>(response->contextPointer());
    
    // Determine the request response roundtrip time
    simtime_t reqSendTime = request->creationTime();
    simtime_t respArriveTime = simTime();
    return (respArriveTime - reqSendTime);
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
