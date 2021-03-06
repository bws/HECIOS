//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include <climits>
#include <cstring>
#include <iostream>
#include "change_dir_ent.h"
#include "collective_create.h"
#include "collective_get_attr.h"
#include "collective_remove.h"
#include "create.h"
#include "create_dir_ent.h"
#include "data_flow.h"
#include "get_attr.h"
#include "bmi_list_io_data_flow.h"
#include "lookup.h"
#include "read_dir.h"
#include "read_pages.h"
#include "read.h"
#include "remove_dir_ent.h"
#include "remove.h"
#include "set_attr.h"
#include "write.h"
#include "pvfs_proto_m.h"
#include "fs_server.h"
using namespace std;

// OMNet Registration Method
Define_Module(FSServer);

size_t FSServer::defaultAttrSize_ = 0;
double FSServer::changeDirEntProcessingDelay_ = 0.0;
double FSServer::createDFileProcessingDelay_ = 0.0;
double FSServer::createDirectoryProcessingDelay_ = 0.0;
double FSServer::createMetadataProcessingDelay_ = 0.0;
double FSServer::createDirEntProcessingDelay_ = 0.0;
double FSServer::getAttrProcessingDelay_ = 0.0;
double FSServer::lookupPathProcessingDelay_ = 0.0;
double FSServer::readDirProcessingDelay_ = 0.0;
double FSServer::removeDirEntProcessingDelay_ = 0.0;
double FSServer::removeMetaProcessingDelay_ = 0.0;
double FSServer::removeObjectProcessingDelay_ = 0.0;
double FSServer::setAttrProcessingDelay_ = 0.0;
double FSServer::serverOverheadDelay_ = 0.0;
bool FSServer::collectDiskData_ = false;

size_t FSServer::getDefaultAttrSize()
{
    return defaultAttrSize_;
}

size_t FSServer::getDirectoryEntrySize()
{
    return 128;
}

void FSServer::setChangeDirEntProcessingDelay(simtime_t changeDirEntDelay)
{
    changeDirEntProcessingDelay_ = changeDirEntDelay.dbl();
}

double FSServer::changeDirEntProcessingDelay()
{
    return changeDirEntProcessingDelay_;
}

void FSServer::setCreateDFileProcessingDelay(simtime_t createDFileDelay)
{
    createDFileProcessingDelay_ = createDFileDelay.dbl();
}

double FSServer::createDFileProcessingDelay()
{
    return createDFileProcessingDelay_;
}

void FSServer::setCreateDirectoryProcessingDelay(simtime_t createDirDelay)
{
    createDirectoryProcessingDelay_ = createDirDelay.dbl();
}

double FSServer::createDirectoryProcessingDelay()
{
    return createDirectoryProcessingDelay_;
}

void FSServer::setCreateMetadataProcessingDelay(simtime_t createMetaDelay)
{
    createMetadataProcessingDelay_ = createMetaDelay.dbl();
}

double FSServer::createMetadataProcessingDelay()
{
    return createMetadataProcessingDelay_;
}

void FSServer::setCreateDirEntProcessingDelay(simtime_t createDirEntDelay)
{
    createDirEntProcessingDelay_ = createDirEntDelay.dbl();
}

double FSServer::createDirEntProcessingDelay()
{
    return createDirEntProcessingDelay_;
}

void FSServer::setGetAttrProcessingDelay(simtime_t getAttrDelay)
{
    getAttrProcessingDelay_ = getAttrDelay.dbl();
}

double FSServer::getAttrProcessingDelay()
{
    return getAttrProcessingDelay_;
}

void FSServer::setLookupPathProcessingDelay(simtime_t lookupPathDelay)
{
    lookupPathProcessingDelay_ = lookupPathDelay.dbl();
}

double FSServer::lookupPathProcessingDelay()
{
    return lookupPathProcessingDelay_;
}

void FSServer::setReadDirProcessingDelay(simtime_t readDirDelay)
{
    readDirProcessingDelay_ = readDirDelay.dbl();
}

double FSServer::readDirProcessingDelay()
{
    return readDirProcessingDelay_;
}

void FSServer::setRemoveDirEntProcessingDelay(simtime_t removeDirEntDelay)
{
    removeDirEntProcessingDelay_ = removeDirEntDelay.dbl();
}

double FSServer::removeDirEntProcessingDelay()
{
    return removeDirEntProcessingDelay_;
}

void FSServer::setRemoveMetaProcessingDelay(simtime_t removeMetaDelay)
{
    removeMetaProcessingDelay_ = removeMetaDelay.dbl();
}

double FSServer::removeMetaProcessingDelay()
{
    return removeMetaProcessingDelay_;
}

void FSServer::setRemoveObjectProcessingDelay(simtime_t removeObjectDelay)
{
    removeObjectProcessingDelay_ = removeObjectDelay.dbl();
}

double FSServer::removeObjectProcessingDelay()
{
    return removeObjectProcessingDelay_;
}

void FSServer::setSetAttrProcessingDelay(simtime_t setAttrDelay)
{
    setAttrProcessingDelay_ = setAttrDelay.dbl();
}

double FSServer::setAttrProcessingDelay()
{
    return setAttrProcessingDelay_;
}

void FSServer::setServerOverheadDelay(simtime_t serverOverheadDelay)
{
    serverOverheadDelay_ = serverOverheadDelay.dbl();
}

void FSServer::setCollectDiskData(bool collectFlag)
{
    collectDiskData_ = collectFlag;
}

FSServer::FSServer()
    : cSimpleModule(),
      changeDirEntDiskDelay_("SPFS Change DirEnt Disk Delay"),
      collectiveCreateDiskDelay_("SPFS Coll Create Disk Delay"),
      collectiveGetAttrDiskDelay_("SPFS Coll GetAttr Disk Delay"),
      collectiveRemoveDiskDelay_("SPFS Coll Remove Disk Delay"),
      createDirEntDiskDelay_("SPFS Create DirEnt Disk Delay"),
      createObjectDiskDelay_("SPFS Create Object Disk Delay"),
      getAttrDiskDelay_("SPFS GetAttr Disk Delay"),
      lookupDiskDelay_("SPFS Lookup Disk Delay"),
      readDirDiskDelay_("SPFS ReadDir Disk Delay"),
      removeDirEntDiskDelay_("SPFS Remove DirEnt Disk Delay"),
      removeObjectDiskDelay_("SPFS Remove Object Disk Delay"),
      setAttrDiskDelay_("SPFS SetAttr Disk Delay")
{
}

bool FSServer::handleIsLocal(const FSHandle& handle) const
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Server checking handle locality: " << handle
    //     << " Range [" << range_.first << "," << range_.last << "]\n";
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
    inGateId_ = findGate("in");
    outGateId_ = findGate("out");

    // Initialize scalar data
    numCollectiveCreates_ = 0;
    numCollectiveGetAttrs_ = 0;
    numCollectiveRemoves_ = 0;
    numChangeDirEnts_ = 0;
    numCreateDirEnts_ = 0;
    numCreateObjects_ = 0;
    numGetAttrs_ = 0;
    numLookups_ = 0;
    numReadDirs_ = 0;
    numReads_ = 0;
    numRemoveObjects_ = 0;
    numRemoveDirEnts_ = 0;
    numSetAttrs_ = 0;
    numWrites_ = 0;
}

void FSServer::finish()
{
    double totalNumOps =
        numCollectiveCreates_ + numCollectiveGetAttrs_ + numCollectiveRemoves_
        + numChangeDirEnts_ + numCreateDirEnts_ + numCreateObjects_
        + numGetAttrs_
        + numLookups_
        + numReadDirs_ + numReads_ + numRemoveObjects_ + numRemoveDirEnts_
        + numSetAttrs_ + numWrites_;

    recordScalar("SPFS Server Operation Total", totalNumOps);
    recordScalar("SPFS Server Collective Creates", numCollectiveCreates_);
    recordScalar("SPFS Server Collective GetAttrs", numCollectiveGetAttrs_);
    recordScalar("SPFS Server Collective Removes", numCollectiveRemoves_);
    recordScalar("SPFS Server ChDirEnts", numChangeDirEnts_);
    recordScalar("SPFS Server CrDirEnts", numCreateDirEnts_);
    recordScalar("SPFS Server CreateObjects", numCreateObjects_);
    recordScalar("SPFS Server GetAttrs", numGetAttrs_);
    recordScalar("SPFS Server Lookups", numLookups_);
    recordScalar("SPFS Server ReadDirs", numReadDirs_);
    recordScalar("SPFS Server Reads", numReads_);
    recordScalar("SPFS Server Removes", numRemoveObjects_);
    recordScalar("SPFS Server RmDirEnts", numRemoveDirEnts_);
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
        cMessage* parentReq = static_cast<cMessage*>(msg->getContextPointer());
        spfsRequest* origRequest =
            static_cast<spfsRequest*>(parentReq->getContextPointer());
        processRequest(origRequest, msg);
        delete parentReq;
        delete msg;
    }
}

void FSServer::processRequest(spfsRequest* request, cMessage* msg)
{
    assert(0 != request);
    switch(request->getKind())
    {
        case SPFS_CHANGE_DIR_ENT_REQUEST:
        {
            ChangeDirEnt changeDirEnt(
                this, static_cast<spfsChangeDirEntRequest*>(request));
            changeDirEnt.handleServerMessage(msg);
            break;
        }
        case SPFS_COLLECTIVE_CREATE_REQUEST:
        {
            CollectiveCreate collCreate(
                this, static_cast<spfsCollectiveCreateRequest*>(request));
            collCreate.handleServerMessage(msg);
            break;
        }
        case SPFS_COLLECTIVE_GET_ATTR_REQUEST:
        {
            CollectiveGetAttr collGetAttr(
                this, static_cast<spfsCollectiveGetAttrRequest*>(request));
            collGetAttr.handleServerMessage(msg);
            break;
        }
        case SPFS_COLLECTIVE_REMOVE_REQUEST:
        {
            CollectiveRemove collRemove(
                this, static_cast<spfsCollectiveRemoveRequest*>(request));
            collRemove.handleServerMessage(msg);
            break;
        }
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
        case SPFS_READ_DIR_REQUEST:
        {
            ReadDir readDir(this, static_cast<spfsReadDirRequest*>(request));
            readDir.handleServerMessage(msg);
            break;
        }
        case SPFS_READ_REQUEST:
        {
            Read read(this, static_cast<spfsReadRequest*>(request));
            read.handleServerMessage(msg);
            break;
        }
        case SPFS_READ_PAGES_REQUEST:
        {
            ReadPages readPages(this, static_cast<spfsReadPagesRequest*>(request));
            readPages.handleServerMessage(msg);
            break;
        }
        case SPFS_REMOVE_DIR_ENT_REQUEST:
        {
            RemoveDirEnt removeDirEnt(
                this, static_cast<spfsRemoveDirEntRequest*>(request));
            removeDirEnt.handleServerMessage(msg);
            break;
        }
        case SPFS_REMOVE_REQUEST:
        {
            Remove remove(this, static_cast<spfsRemoveRequest*>(request));
            remove.handleServerMessage(msg);
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
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "FSServer Error: Unknown message kind:" << request->getKind()
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
    cSimpleModule::sendDelayed(msg, delay + serverOverheadDelay_, outGateId_);
}

void FSServer::recordChangeDirEnt()
{
    numChangeDirEnts_++;
}

void FSServer::recordCollectiveCreate()
{
    numCollectiveCreates_++;
}

void FSServer::recordCollectiveGetAttr()
{
    numCollectiveGetAttrs_++;
}

void FSServer::recordCollectiveRemove()
{
    numCollectiveRemoves_++;
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

void FSServer::recordReadDir()
{
    numReadDirs_++;
}

void FSServer::recordReadPages()
{
    numReadPages_++;
}

void FSServer::recordRead()
{
    numReads_++;
}

void FSServer::recordRemoveDirEnt()
{
    numRemoveDirEnts_++;
}

void FSServer::recordRemoveObject()
{
    numRemoveObjects_++;
}

void FSServer::recordSetAttr()
{
    numSetAttrs_++;
}

void FSServer::recordWrite()
{
    numWrites_++;
}

void FSServer::recordChangeDirEntDiskDelay(cMessage* fileWriteResponse)
{
    if (collectDiskData_)
    {
        changeDirEntDiskDelay_.record(getRoundTripDelay(fileWriteResponse));
    }
}

void FSServer::recordCreateDirEntDiskDelay(cMessage* fileWriteResponse)
{
    if (collectDiskData_)
    {
        createDirEntDiskDelay_.record(getRoundTripDelay(fileWriteResponse));
    }
}

void FSServer::recordCreateObjectDiskDelay(cMessage* fileOpenResponse)
{
    if (collectDiskData_)
    {
        createObjectDiskDelay_.record(getRoundTripDelay(fileOpenResponse));
    }
}

//void FSServer::recordReadDataDiskDelay(cMessage* fileReadResponse)
//{
//    if (collectDiskData_)
//    {
//        readDataDiskDelay_.record(getRoundTripDelay(fileReadResponse));
//    }
//}

void FSServer::recordGetAttrDiskDelay(cMessage* fileReadResponse)
{
    if (collectDiskData_)
    {
        getAttrDiskDelay_.record(getRoundTripDelay(fileReadResponse));
    }
}

void FSServer::recordLookupDiskDelay(cMessage* fileReadResponse)
{
    if (collectDiskData_)
    {
        lookupDiskDelay_.record(getRoundTripDelay(fileReadResponse));
    }
}

void FSServer::recordReadDirDiskDelay(cMessage* fileReadResponse)
{
    if (collectDiskData_)
    {
        readDirDiskDelay_.record(getRoundTripDelay(fileReadResponse));
    }
}

void FSServer::recordRemoveDirEntDiskDelay(cMessage* fileWriteResponse)
{
    if (collectDiskData_)
    {
        removeDirEntDiskDelay_.record(getRoundTripDelay(fileWriteResponse));
    }
}

void FSServer::recordRemoveObjectDiskDelay(cMessage* fileUnlinkResponse)
{
    if (collectDiskData_)
    {
        removeObjectDiskDelay_.record(getRoundTripDelay(fileUnlinkResponse));
    }
}

void FSServer::recordSetAttrDiskDelay(cMessage* fileWriteResponse)
{
    if (collectDiskData_)
    {
        setAttrDiskDelay_.record(getRoundTripDelay(fileWriteResponse));
    }
}

//void FSServer::recordWriteDataDiskDelay(cMessage* fileWriteResponse)
//{
//    if (collectDiskData_)
//    {
//        writeDataDiskDelay_.record(getRoundTripDelay(fileWriteResponse));
//    }
//}

simtime_t FSServer::getRoundTripDelay(cMessage* response) const
{
    // Get the originating request
    cMessage* request = static_cast<cMessage*>(response->getContextPointer());

    // Determine the request response roundtrip time
    simtime_t reqSendTime = request->getCreationTime();
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
