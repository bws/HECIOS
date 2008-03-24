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
#include "fs_read_directory.h"
#include "fs_stat.h"
#include "fs_update_time.h"
#include "fs_write.h"
#include "pfs_types.h"
#include "pvfs_proto_m.h"
#include "mpi_proto_m.h"
using namespace std;

// Define FSClient module for this class
Define_Module(FSClient);

spfsCollectiveCreateRequest* FSClient::createCollectiveCreateRequest(
    const FSHandle& handle, vector<FSHandle> dataHandles)
{
    spfsCollectiveCreateRequest* create =
        new spfsCollectiveCreateRequest(0, SPFS_COLLECTIVE_CREATE_REQUEST);
    create->setHandle(handle);
    create->setObjectType(SPFS_METADATA_OBJECT);

    // Add the data handles
    create->setDataHandlesArraySize(dataHandles.size());
    for (size_t i = 0; i < dataHandles.size(); i++)
    {
        create->setDataHandles(i, dataHandles[i]);
    }

    // Set the collective create request size (op, creds, fs_id, objType,
    //  metahandle, numExtentArrays, extentArraySizes, extentArrays)
    long msgSize = 4 + FSClient::CREDENTIALS_SIZE + 4 + 4 + 8 +
        4 + dataHandles.size()*4 + dataHandles.size()*8;
    create->setByteLength(msgSize);
    return create;
}

spfsCreateRequest* FSClient::createCreateRequest(const FSHandle& handle,
                                                 FSObjectType objectType)
{
    spfsCreateRequest* create = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    create->setHandle(handle);
    create->setObjectType(objectType);

    // Set the create request size (op, creds, fs_id, objType, extentArraySize,
    // extentArray)
    create->setByteLength(4 + FSClient::CREDENTIALS_SIZE + 4 + 4 + 4 + 8);    
    return create;
}

spfsCreateDirEntRequest* FSClient::createCreateDirEntRequest(
    const FSHandle& handle, const Filename& entry)
{
    spfsCreateDirEntRequest* createDirEnt =
        new spfsCreateDirEntRequest(0, SPFS_CREATE_DIR_ENT_REQUEST);
    createDirEnt->setHandle(handle);
    createDirEnt->setEntry(entry.c_str());

    // Set the create dirent request size (op, creds, fs_id, entry,
    // newHandle, parentHandle)
    createDirEnt->setByteLength(4 + FSClient::CREDENTIALS_SIZE + 4 +
                                entry.str().length() + 1 + 8 + 8);
    return createDirEnt;
}

spfsGetAttrRequest* FSClient::createGetAttrRequest(const FSHandle& handle,
                                                   FSObjectType objectType)
{
    spfsGetAttrRequest* getAttr = new spfsGetAttrRequest(0,
                                                         SPFS_GET_ATTR_REQUEST);
    getAttr->setHandle(handle);
    getAttr->setObjectType(objectType);

    // Set the get attr request size (op, creds, fs_id, handle, attrMask)
    getAttr->setByteLength(4 + FSClient::CREDENTIALS_SIZE + 4 + 8 + 4);
    return getAttr;
}

spfsLookupPathRequest* FSClient::createLookupPathRequest(
        const Filename& lookupName,
        const FSHandle& handle,
        int numResolvedSegments)
{
    spfsLookupPathRequest* lookup = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    lookup->setFilename(lookupName.c_str());
    lookup->setHandle(handle);
    lookup->setNumResolvedSegments(numResolvedSegments);
    lookup->setLocallyResolvedSegments(0);

    // Set the lookup request size (op, creds, fs_id, pathSize, path, handle,
    // attrMask)
    lookup->setByteLength(4 + FSClient::CREDENTIALS_SIZE + 4 +
                          4 + lookupName.str().length() + 8 + 4);
    return lookup;
}

spfsReadDirRequest* FSClient::createReadDirRequest(const FSHandle& handle,
                                                   size_t dirEntCount)
{
    spfsReadDirRequest* readDir = new spfsReadDirRequest(0,
                                                         SPFS_READ_DIR_REQUEST);
    readDir->setHandle(handle);
    readDir->setDirOffset(0);
    readDir->setDirEntCount(dirEntCount);

    readDir->setByteLength(4 + FSClient::CREDENTIALS_SIZE + 4 + 8 + 8 + 8);

    return readDir;
}

spfsReadRequest* FSClient::createReadRequest(const FSHandle& handle,
                                             const FileView& view,
                                             FSOffset offset,
                                             FSSize dataSize,
                                             const FileDistribution& dist)
{
    spfsReadRequest* read = new spfsReadRequest(0, SPFS_READ_REQUEST);
    read->setHandle(handle);
    read->setView(new FileView(view));
    read->setOffset(offset);
    read->setDataSize(dataSize);
    read->setDist(dist.clone());
    read->setClientFlowBmiTag(simulation.getUniqueNumber());
    read->setServerFlowBmiTag(simulation.getUniqueNumber());
    
    // Set the Read request size (op, creds, fs_id, handle, objType,
    // attrMask, Attributes) TODO: fix attribute size
    read->setByteLength(4 + FSClient::CREDENTIALS_SIZE + 4 +
                        8 + 4 + 4 + 4 +
                        view.getRepresentationByteLength() +
                        8 + 8);
    return read;
}

spfsSetAttrRequest* FSClient::createSetAttrRequest(const FSHandle& handle,
                                                   FSObjectType objectType)
{
    spfsSetAttrRequest* setAttr =
        new spfsSetAttrRequest(0, SPFS_SET_ATTR_REQUEST);
    setAttr->setHandle(handle);
    setAttr->setObjectType(objectType);

    // Set the SetAttr request size (op, creds, fs_id, handle, objType,
    // attrMask, Attributes) TODO: fix attribute size
    setAttr->setByteLength(4 + FSClient:: CREDENTIALS_SIZE + 4 + 8 +
                           4 + 4 + 64);
    return setAttr;
}

spfsWriteRequest* FSClient::createWriteRequest(const FSHandle& handle,
                                               const FileView& view,
                                               FSOffset offset,
                                               FSSize dataSize,
                                               const FileDistribution& dist)
{
    spfsWriteRequest* write = new spfsWriteRequest(0, SPFS_WRITE_REQUEST);
    write->setHandle(handle);
    write->setView(new FileView(view));
    write->setOffset(offset);
    write->setDataSize(dataSize);
    write->setDist(dist.clone());
    write->setClientFlowBmiTag(simulation.getUniqueNumber());
    write->setServerFlowBmiTag(simulation.getUniqueNumber());
    
    // Set the Write request size (op, creds, fs_id, handle, objType,
    // attrMask, Attributes) TODO: fix attribute size
    write->setByteLength(4 + FSClient::CREDENTIALS_SIZE + 4 +
                         8 + 4 + 4 + 4 +
                         view.getRepresentationByteLength() +
                         8 + 8);
    return write;
}

FSClient::FSClient()
    : collectiveCreateDelay_("SPFS Collective Create Roundtrip Delay"),
      createDirEntDelay_("SPFS Client CrDirEnt Roundtrip Delay"),
      createObjectDelay_("SPFS Client CreateObject Roundtrip Delay"),
      flowDelay_("SPFS Client Flow Delay"),
      getAttrDelay_("SPFS Client GetAttr Roundtrip Delay"),
      lookupPathDelay_("SPFS Client Lookup Path Roundtrip Delay"),
      readDirDelay_("SPFS Client Read Dir Roundtrip Delay"),
      readDelay_("SPFS Client Read Roundtrip Delay"),
      removeDelay_("SPFS Client Remove Roundtrip Delay"),
      setAttrDelay_("SPFS Client SetAttr Roundtrip Delay"),
      writeCompleteDelay_("SPFS Client WriteComplete Roundtrip Delay"),
      writeDelay_("SPFS Client Write Roundtrip Delay")
{
}

void FSClient::initialize()
{
    // Determine gate ids
    appInGateId_ = findGate("appIn");
    appOutGateId_ = findGate("appOut");
    netInGateId_ = findGate("netIn");
    netOutGateId_ = findGate("netOut");

    // Retrieve client optimizations
    useCollectiveCreate_ = par("useCollectiveCreate");
    useCollectiveGetAttr_ = par("useCollectiveGetAttr");
    useCollectiveRemove_ = par("useCollectiveRemove");

    // Retrieve processing delays
    directoryCreateProcessingDelay_ = par("directoryCreateProcessingDelaySecs");
    directoryReadProcessingDelay_ = par("directoryReadProcessingDelaySecs");
    directoryRemoveProcessingDelay_ = par("directoryRemoveProcessingDelaySecs");
    fileCloseProcessingDelay_ = par("fileCloseProcessingDelaySecs");
    fileOpenProcessingDelay_ = par("fileOpenProcessingDelaySecs");
    fileReadProcessingDelay_ = par("fileReadProcessingDelaySecs");
    fileStatProcessingDelay_ = par("fileStatProcessingDelaySecs");
    fileUpdateTimeProcessingDelay_ = par("fileUpdateTimeProcessingDelaySecs");
    fileWriteProcessingDelay_ = par("fileWriteProcessingDelaySecs");

    // Initialize scalar record data
    numACacheHits_ = 0;
    numACacheMisses_ = 0;
    numDCacheHits_ = 0;
    numDCacheMisses_ = 0;
    numDirCreates_ = 0;
    numDirReads_ = 0;
    numFileCloses_ = 0;
    numFileOpens_ = 0;
    numFileReads_ = 0;
    numFileWrites_ = 0;
    numFileUtimes_ = 0;
}

void FSClient::finish()
{
    recordScalar("SPFS Dir Creates", numDirCreates_);
    recordScalar("SPFS Dir Reads", numDirReads_);
    recordScalar("SPFS File Closes", numFileCloses_);
    recordScalar("SPFS File Opens", numFileOpens_);
    recordScalar("SPFS File Reads", numFileReads_);
    recordScalar("SPFS File Stats", numFileStats_);
    recordScalar("SPFS File Utimes", numFileUtimes_);
    recordScalar("SPFS File Writes", numFileWrites_);
}

void FSClient::handleMessage(cMessage *msg)
{
    // If the message is from the application, schedule it with
    // its associated processing delay
    // Else if its a self message, then it has been delayed, and will
    //   need to be processed immediately
    // Else its a response and needs to be processed immediately
    if (msg->arrivalGateId() == appInGateId_)
    {
        // Account for the request processing delay
        scheduleRequest(msg);
    }
    else if (msg->isSelfMessage())
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

void FSClient::scheduleRequest(cMessage* request)
{
    simtime_t scheduleTime = simTime();
    switch(request->kind())
    {
        case SPFS_MPI_DIRECTORY_CREATE_REQUEST:
        {
            numDirCreates_++;
            scheduleTime += directoryCreateProcessingDelay_;
            break;
        }
        case SPFS_MPI_DIRECTORY_READ_REQUEST:
        {
            numDirReads_++;
            scheduleTime += directoryReadProcessingDelay_;
            break;
        }
        case SPFS_MPI_DIRECTORY_REMOVE_REQUEST:
        {
            numDirRemoves_++;
            scheduleTime += directoryRemoveProcessingDelay_;
            break;
        }
        case SPFS_MPI_FILE_OPEN_REQUEST:
        {
            numFileOpens_++;
            scheduleTime += fileOpenProcessingDelay_;
            break;
        }
        case SPFS_MPI_FILE_CLOSE_REQUEST :
        {
            numFileCloses_++;
            scheduleTime += fileCloseProcessingDelay_;
            break;
        }
        case SPFS_MPI_FILE_READ_AT_REQUEST:
        case SPFS_MPI_FILE_READ_REQUEST:
        case SPFS_MPI_FILE_IREAD_REQUEST:
        {
            numFileReads_++;
            scheduleTime += fileReadProcessingDelay_;
            break;
        }
        case SPFS_MPI_FILE_STAT_REQUEST:
        {
            numFileStats_++;
            scheduleTime += fileStatProcessingDelay_;
            break;
        }
        case SPFS_MPI_FILE_UPDATE_TIME_REQUEST:
        {
            numFileUtimes_++;
            scheduleTime += fileUpdateTimeProcessingDelay_;
            break;
        }
        case SPFS_MPI_FILE_WRITE_AT_REQUEST:
        case SPFS_MPI_FILE_WRITE_REQUEST:
        case SPFS_MPI_FILE_IWRITE_REQUEST:
        {
            numFileWrites_++;
            scheduleTime += fileWriteProcessingDelay_;
            break;
        }
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Unknown Message: " << request->kind()
                 << " " << request->info() << endl;
            break;
        }
    }
    scheduleAt(scheduleTime, request);
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
        case SPFS_MPI_DIRECTORY_READ_REQUEST:
        {
            FSReadDirectory readDir(
                this, static_cast<spfsMPIDirectoryReadRequest*>(request));
            readDir.handleMessage(msg);
            break;
        }
        case SPFS_MPI_FILE_OPEN_REQUEST:
        {
            FSOpen open(this,
                        static_cast<spfsMPIFileOpenRequest*>(request),
                        useCollectiveCreate_);
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
        case SPFS_MPI_FILE_STAT_REQUEST:
        {
            FSStat stat(
                this, static_cast<spfsMPIFileStatRequest*>(request));
            stat.handleMessage(msg);
            break;
        }
        case SPFS_MPI_FILE_UPDATE_TIME_REQUEST:
        {
            FSUpdateTime utime(
                this, static_cast<spfsMPIFileUpdateTimeRequest*>(request));
            utime.handleMessage(msg);
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
        case SPFS_MPI_FILE_IREAD_REQUEST:
        {
            FSRead read(this,
                        static_cast<spfsMPIFileReadAtRequest*>(request));
            read.handleMessage(msg);
            spfsMPIFileReadResponse *readmsg = new spfsMPIFileReadResponse(
                0, SPFS_MPI_FILE_IREAD_RESPONSE);
            readmsg->setContextPointer(request->dup());
            send(readmsg, appOutGateId_);                                                                           
            break;
        }
        case SPFS_MPI_FILE_WRITE_REQUEST:
        {
            cerr << "ERROR: Illegal write message!" << endl;
            break;
        }
        case SPFS_MPI_FILE_IWRITE_REQUEST:
        {
            FSWrite write(this,
                          static_cast<spfsMPIFileWriteAtRequest*>(request));
            write.handleMessage(msg);
            spfsMPIFileWriteResponse *writemsg = new spfsMPIFileWriteResponse(
                0, SPFS_MPI_FILE_IWRITE_RESPONSE);
            writemsg->setContextPointer(request->dup());
            send(writemsg, appOutGateId_);
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
    // Determine the request response roundtrip time
    cMessage* parentRequest =
        static_cast<cMessage*>(serverResponse->contextPointer());
    simtime_t reqSendTime = parentRequest->creationTime();
    simtime_t respArriveTime = simTime();
    simtime_t delay = respArriveTime - reqSendTime;
    
    switch(serverResponse->kind())
    {
        case SPFS_COLLECTIVE_CREATE_RESPONSE:
        {
            collectiveCreateDelay_.record(delay);
            break;
        }
        case SPFS_CREATE_DIR_ENT_RESPONSE:
        {
            createDirEntDelay_.record(delay);
            break;
        }
        case SPFS_CREATE_RESPONSE:
        {
            createObjectDelay_.record(delay);
            break;
        }
        case SPFS_DATA_FLOW_FINISH:
        {
            flowDelay_.record(delay);
            break;
        }
        case SPFS_GET_ATTR_RESPONSE:
        {
            getAttrDelay_.record(delay);
            break;
        }
        case SPFS_LOOKUP_PATH_RESPONSE:
        {
            lookupPathDelay_.record(delay);
            break;
        }
        case SPFS_READ_DIR_RESPONSE:
        {
            readDirDelay_.record(delay);
            break;
        }
        case SPFS_READ_RESPONSE:
        {
            readDelay_.record(delay);
            break;
        }
        case SPFS_SET_ATTR_RESPONSE:
        {
            setAttrDelay_.record(delay);
            break;
        }
        case SPFS_WRITE_COMPLETION_RESPONSE:
        {
            writeCompleteDelay_.record(delay);
            break;
        }
        case SPFS_WRITE_RESPONSE:
        {
            writeDelay_.record(delay);
            break;
        }
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "Unable to collect data for message: "
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
