//
// This file is part of Hecios
//
// Copyright (C) 2007 Bradley W. Settlemyer
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
#include "phtf_io_application.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include "basic_data_type.h"
#include "contiguous_data_type.h"
#include "struct_data_type.h"
#include "subarray_data_type.h"
#include "vector_data_type.h"
#include "cache_proto_m.h"
#include "filename.h"
#include "file_builder.h"
#include "file_descriptor.h"
#include "middleware_cache.h"
#include "mpi_proto_m.h"
#include "storage_layout_manager.h"
#include "phtf_io_trace.h"
#include "comm_man.h"

using namespace std;

// OMNet Registration Method
Define_Module(PHTFIOApplication);

PHTFIOApplication::PHTFIOApplication()
    : IOApplication(),
      phtfEvent_(0)
{
}

/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void PHTFIOApplication::initialize()
{
    // Parent class initialization
    IOApplication::initialize();

    // Retrieve the CPU pause flag
    disableCPUPause_ = par("disableCPUPhase").boolValue();

    // Retrieve the trace printing flag
    printTrace_ = par("isVerbose").boolValue();

    // Retrieve the resource describing the trace location
    traceDirectory_ = par("traceFile").stringValue();
    bool traceIsEnabled = (0 != traceDirectory_.size());

    if (traceIsEnabled)
    {
        // PHTF stuff not able to support single initialization, so
        // enforce that locally
        static bool phtfInit = false;
        if (!phtfInit)
        {
            // Set the trace directory (and initialize the file system config)
            PHTFTrace::instance().dirPath(traceDirectory_);

            // Set value for MPI_COMM_WORLD & MPI_COMM_SELF based on configuration
            // in fs.ini
            string commWorldValue = PHTFTrace::instance().getFs()->consts("MPI_COMM_WORLD");
            CommMan::instance().setCommWorld(strtol(commWorldValue.c_str(), 0, 0));

            string commSelfValue = PHTFTrace::instance().getFs()->consts("MPI_COMM_SELF");
            CommMan::instance().setCommSelf(strtol(commSelfValue.c_str(), 0, 0));

            // Build a singleton map of mpi operation IDs (string => ID)
            PHTFEventRecord::buildOpMap();

            // Disable further initialization
            phtfInit = true;
        }

        // Schedule the kick start message
        double maxBeginTime = par("maxBeginTime").doubleValue();
        cMessage* kickStart = new cMessage(CPU_PHASE_MESSAGE_NAME);
        double kickStartTime = uniform(0.0, maxBeginTime);
        scheduleAt(kickStartTime, kickStart);
    }
    else
    {
        static bool printOnce = false;
        if (!printOnce)
        {
            cout << "MPI-IO Tracing is disabled.\n";
            printOnce = true;
        }
    }
}

/**
 * Cleanup trace and tally statistics
 */
void PHTFIOApplication::finish()
{
    IOApplication::finish();

    if (0 != traceDirectory_.size())
    {
        // Aggregate accumulation statistics
        static size_t numResults = 0;
        static double aggReadTime = 0.0;
        static double aggReadBytes = 0.0;
        static double aggWriteTime = 0.0;
        static double aggWriteBytes = 0.0;
        static double maxReadTime = 0.0;
        static double maxWriteTime = 0.0;

        // Accumulate the aggregate bandwidth
        numResults++;
        aggReadBytes += getNumReadBytes();
        aggReadTime += getReadTime();
        aggWriteBytes += getNumWriteBytes();
        aggWriteTime += getWriteTime();
        maxReadTime = max(maxReadTime, getReadTime());
        maxWriteTime = max(maxWriteTime, getWriteTime());

        // Write out the aggregate statistic on the final process
        if (numResults == CommMan::instance().commSize(SPFS_COMM_WORLD))
        {
            double aggWriteBandwidth = aggWriteBytes / (1.0e6 * maxWriteTime);
            recordScalar("SPFS Aggregate Write Bandwidth", aggWriteBandwidth);
            cout << "Aggregated Write bandwidth: " << aggWriteBandwidth << endl;

            double aggReadBandwidth = aggReadBytes / (1.0e6 * maxReadTime);
            recordScalar("SPFS Aggregate Read Bandwidth", aggReadBandwidth);
            cout << "Aggregated Read bandwidth: " << aggReadBandwidth << endl;
        }

        // Cleanup trace resources
        phtfEvent_->close();
    }
}

void PHTFIOApplication::rankChanged(int oldRank)
{
    // Ensure we only join the communicator once
    assert(-1 == oldRank);

    // Join the world communicator on rank initialization
    CommMan::instance().registerRank(getRank());
}

void PHTFIOApplication::populateBasicDataTypes()
{
}

void PHTFIOApplication::populateFileSystem()
{
    cout << "Populating file system . . . " << flush;
    PHTFFs *fs = PHTFTrace::instance().getFs();
    int numFiles = fs->fileNum();
    FileSystemMap fsm;
    for(int i = 0; i < numFiles; i ++)
    {
        fsm[fs->fileName(i)] = fs->fileSize(i);
    }
    FileBuilder::instance().populateFileSystem(fsm);
    cout << "Done." << endl << flush;
}

void PHTFIOApplication::handleMessage(cMessage* msg)
{
    // Add code to allow opens to finish by broadcasting result
    if (SPFS_MPI_FILE_OPEN_RESPONSE == msg->kind())
    {
        // Extract the communicator
        spfsMPIFileOpenRequest* openRequest =
            static_cast<spfsMPIFileOpenRequest*>(msg->contextPointer());
        assert(0 != openRequest);
        Communicator commId = openRequest->getCommunicator();

        // Broadcast result to the remainder of the communicator if necessary
        if (SPFS_COMM_SELF != commId)
        {
            cMessage* bcast = createBcastRequest(commId);
            send(bcast, mpiOutGate_);

            // Cleanup the open request and response
            delete openRequest;
            delete msg;
        }
        else
        {
            IOApplication::handleMessage(msg);
        }
    }
    else
    {
        IOApplication::handleMessage(msg);
    }
}

void PHTFIOApplication::handleMPIMessage(cMessage* msg)
{
    // Cleanup the message
    cMessage* originatingRequest =
        static_cast<cMessage*>(msg->contextPointer());
    delete originatingRequest;
    delete msg;
}

void PHTFIOApplication::handleIOMessage(cMessage* msg)
{
    cMessage* originatingRequest = (cMessage*)msg->contextPointer();
    assert(0 != originatingRequest);


    if (originatingRequest->kind() == SPFS_MPI_FILE_WRITE_AT_REQUEST)
    {
        int requestId = static_cast<spfsMPIFileWriteAtRequest*>(
            originatingRequest)->getReqId();
        pendingRequestsById_.erase(requestId);
    }
    else if(originatingRequest->kind() == SPFS_MPI_FILE_READ_AT_REQUEST)
    {
        int requestId = dynamic_cast<spfsMPIFileReadAtRequest*>(
            originatingRequest)->getReqId();
        pendingRequestsById_.erase(requestId);
    }

    // Let the IOApplication finish handling the message
    IOApplication::handleIOMessage(msg);
}

bool PHTFIOApplication::scheduleNextMessage()
{
    // Open the trace if this is the first invokation
    if(!phtfEvent_)
    {
        // get event file for this process
        phtfEvent_ = PHTFTrace::instance().getEvent(getRank());
        assert(0 != phtfEvent_);
        phtfEvent_->open();
    }

    bool msgScheduled = false;
    if (!phtfEvent_->eof())
    {
        PHTFEventRecord eventRecord;
        *phtfEvent_ >> eventRecord;

        if (printTrace_)
        {
            cout << "[" << getRank() << "] " << eventRecord.recordStr() << endl;
            cout.flush();
        }
        rec_id_ = eventRecord.recordId();
        msgScheduled = processEvent(eventRecord);
    }
    return msgScheduled;
}

bool PHTFIOApplication::processEvent(PHTFEventRecord& eventRecord)
{
    bool msgScheduled = false;
    int opType = eventRecord.recordOp();
    switch(opType)
    {
        case CART_CREATE:
        case CART_GET:
        case COMM_DUP:
        case COMM_RANK:
        case CPU_PHASE:
        case IREAD:
        case IWRITE:
        case OPEN:
        case SEEK:
        case SET_VIEW:
        case SYNC:
        case TYPE_CONTIGUOUS:
        case TYPE_CREATE_SUBARRAY:
        case WAIT:
        {
            // Events processed on the client, or with multiple steps
            msgScheduled = processIrregularEvent(&eventRecord);
            break;
        }
        case ALLREDUCE:
        case BARRIER:
        case BCAST:
        {
            // MPI messaging events
            cMessage* request = createMessage(&eventRecord);
            send(request, mpiOutGate_);
            msgScheduled = true;
            break;
        }
        default:
        {
            // Normal file processing events
            cMessage* request = createMessage(&eventRecord);
            send(request, ioOutGate_);
            msgScheduled = true;
        }
    }
    return msgScheduled;
}

bool PHTFIOApplication::processIrregularEvent(PHTFEventRecord* eventRecord)
{
    bool msgScheduled = false;
    int opType = eventRecord->recordOp();
    switch(opType)
    {
        case CPU_PHASE:
        {
            if (disableCPUPause_)
            {
                // Skip to the next message
                msgScheduled = scheduleNextMessage();
            }
            else
            {
                cMessage* msg = createCPUPhaseMessage(eventRecord);
                scheduleCPUMessage(msg);
                msgScheduled = true;
            }
            break;
        }
        case COMM_DUP:
        {
            // Retrieve the next operation, as this one is local
            performCommDup(*eventRecord);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case OPEN:
        {
            // Perform the open processing
            Communicator commId = 0;
            performOpenProcessing(eventRecord, commId);
            assert(0 != commId);

            // Determine if this is first rank for the communicator
            int openRank = CommMan::instance().commRank(commId, getRank());
            if (0 == openRank)
            {
                cMessage* msg = createFileOpenMessage(eventRecord);
                send(msg, ioOutGate_);
            }
            else
            {
                //perform the open for the non-leader nodes
                performFakeOpenProcessing(*eventRecord);

                // Perform the broadcast to complete the open
                cMessage* msg = createBcastRequest(commId);
                send(msg, mpiOutGate_);
            }
            msgScheduled = true;
            break;
        }
        case SEEK:
        {
            // Perform seek operation
            performSeekProcessing(eventRecord);

            // Retrieve the next operation, as this one is local
            msgScheduled = scheduleNextMessage();
            break;
        }
        case SET_VIEW:
        {
            performFileSetView(*eventRecord);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case SYNC:
        {
            //TODO: At present we ignore syncs -- not the right thing
            msgScheduled = scheduleNextMessage();
            break;
        }
        case TYPE_CONTIGUOUS:
        {
            performTypeContiguous(*eventRecord);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case TYPE_CREATE_SUBARRAY:
        {
            performTypeCreateSubarray(*eventRecord);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case IREAD:
        {
            // Start the non-blocking operation
            cMessage* msg = createFileIReadMessage(eventRecord);
            send(msg, ioOutGate_);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case IWRITE:
        {
            // Start the non-blocking operation
            cMessage* msg = createFileIWriteMessage(eventRecord);
            send(msg, ioOutGate_);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case WAIT:
        {
            bool waitIsComplete = false;
            performWaitProcessing(eventRecord, waitIsComplete);

            // If the wait operation completes immediately, schedule the
            // next message now, otherwise just wait on the response
            if (waitIsComplete)
            {
                msgScheduled = scheduleNextMessage();
            }
            else
            {
                msgScheduled = true;
            }
            break;
        }
        case CART_CREATE:
        {
            performCartCreate(*eventRecord);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case CART_GET:
        {
            performCartGet(*eventRecord);
            msgScheduled = scheduleNextMessage();
            break;
        }
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Unable to handle irregular event: "
                 << eventRecord->recordStr() << endl;
            assert(false);
            _Exit(25);
        }
    }
    return msgScheduled;
}

cMessage* PHTFIOApplication::createMessage(const PHTFEventRecord* eventRecord)
{
    // Create the request for each event record type
    cMessage* request = 0;
    int opType = eventRecord->recordOp();
    switch(opType)
    {
        case ALLREDUCE:
        {
            request = createAllReduceMessage(eventRecord);
            break;
        }
        case BARRIER:
        {
            request = createBarrierMessage(eventRecord);
            break;
        }
        case BCAST:
        {
            request = createBcastMessage(eventRecord);
            break;
        }
        case CLOSE:
        {
            request = createFileCloseMessage(eventRecord);
            break;
        }
        case DELETE:
        {
            request = createFileDeleteMessage(eventRecord);
            break;
        }
        case GET_INFO:
        {
            request = createFileGetInfoMessage(eventRecord);
            break;
        }
        case GET_SIZE:
        {
            request = createFileGetSizeMessage(eventRecord);
            break;
        }
        case READ:
        {
            request = createFileReadMessage(eventRecord);
            break;
        }
        case READ_ALL:
        {
            request = createFileReadAllMessage(eventRecord);
            break;
        }
        case READ_AT:
        {
            request = createFileReadAtMessage(eventRecord);
            break;
        }
        case SET_SIZE:
        {
            request = createFileSetSizeMessage(eventRecord);
            break;
        }
        case WRITE:
        {
            request = createFileWriteMessage(eventRecord);
            break;
        }
        case WRITE_ALL:
        {
            request = createFileWriteAllMessage(eventRecord);
            break;
        }
        case WRITE_AT:
        {
            request = createFileWriteAtMessage(eventRecord);
            break;
        }
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Unable to handle event: "
                 << eventRecord->recordStr() << endl;
            _Exit(25);
        }
    }
    assert(0 != request);
    return request;
}

void PHTFIOApplication::scheduleCPUMessage(cMessage *msg)
{
    double schTime = simTime() + msg->par("Delay").doubleValue();
    scheduleAt(schTime , msg);
}

void PHTFIOApplication::performCartCreate(const PHTFEventRecord& cartCreate)
{
    // In this case a simple duplication of the communicator should suffice
    Communicator oldComm = cartCreate.paramAsAddress(0);
    Communicator newComm = cartCreate.paramAsAddress(5);
    assert(true == CommMan::instance().exists(oldComm));
    cerr << "Duplicating comm: " << oldComm << " as new: " << newComm << endl;
    CommMan::instance().dupComm(oldComm, newComm);
}

void PHTFIOApplication::performCartGet(const PHTFEventRecord& openRecord)
{
    // No-op
}

void PHTFIOApplication::performCommDup(const PHTFEventRecord& commDup)
{
    Communicator oldComm = commDup.paramAsAddress(0);
    Communicator newComm = commDup.paramAsAddress(1);
    assert(true == CommMan::instance().exists(oldComm));
    CommMan::instance().dupComm(oldComm, newComm);
}

void PHTFIOApplication::performFakeOpenProcessing(const PHTFEventRecord& openRecord)
{
    // Extract the descriptor number from the event record
    int handle = openRecord.paramAsDescriptor(4, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);
    assert(0 != fd);

    // This is a brutal hack to work around the open-bcast optimization
    // In effect, we'll simply tell the cache directly to open the file
    // for its own record keeping
    cModule* mpiProcess = parentModule();
    assert(0 != mpiProcess);
    cModule* cacheModule = mpiProcess->submodule("cache");
    assert(0 != cacheModule);
    MiddlewareCache* middlewareCache = dynamic_cast<MiddlewareCache*>(cacheModule);
    assert(0 != middlewareCache);
    middlewareCache->performFakeOpen(fd->getFilename());
}

void PHTFIOApplication::performOpenProcessing(PHTFEventRecord* openRecord,
                                              Communicator& outCommunicatorId)
{
    // Extract the filename
    Filename openFilename(openRecord->paramAt(1));

    // Extract the descriptor number from the event record
    int fileId = openRecord->paramAsDescriptor(4, *phtfEvent_);

    // Extract the communicator id
    outCommunicatorId = openRecord->paramAsAddress(0);

    // Construct a file descriptor for use in simulation
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(openFilename);
    fd->setCommunicator(outCommunicatorId);
    assert(0 != fd);

    // Associate the file id with a file descriptor
    setDescriptor(fileId, fd);
}

void PHTFIOApplication::performSeekProcessing(PHTFEventRecord* seekRecord)
{
    int handle = seekRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    size_t offset = seekRecord->paramAsSizeT(1);
    int whence = seekRecord->paramAsSizeT(2);

    // Determine the seek whence values
    int MPI_SEEK_SET, MPI_SEEK_CUR, MPI_SEEK_END;
    PHTFTrace* trace = &(PHTFTrace::instance());
    istringstream seekSetStream(trace->getFs()->consts("MPI_SEEK_SET"));
    seekSetStream >> MPI_SEEK_SET;
    istringstream seekCurStream(trace->getFs()->consts("MPI_SEEK_CUR"));
    seekCurStream >> MPI_SEEK_CUR;
    istringstream seekEndStream(trace->getFs()->consts("MPI_SEEK_END"));
    seekEndStream >> MPI_SEEK_END;

    // Adjust the file pointer
    if (MPI_SEEK_SET == whence)
    {
        fd->setFilePointer(offset);
    }
    else if (MPI_SEEK_CUR == whence)
    {
        fd->moveFilePointer(offset);
    }
    else
    {
        assert(MPI_SEEK_END == whence);
        fd->setFilePointer(fd->getMetaData()->size + offset);
    }
}

void PHTFIOApplication::performWaitProcessing(PHTFEventRecord* waitRecord,
                                              bool& outWaitIsComplete)
{
    // Extract the request id
    uint64_t requestId = waitRecord->paramAsAddress(0);

    // Determine if the operation being waiting on is pending
    outWaitIsComplete = false;
    if (0 == pendingRequestsById_.count(requestId))
    {
        outWaitIsComplete = true;
    }
}

void PHTFIOApplication::performFileSetView(const PHTFEventRecord& fileSetView)
{
    // Retrieve the descriptor
    int handle = fileSetView.paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);
    assert(0 != fd);

    // Construct the new file view
    size_t displacement = fileSetView.paramAsSizeT(1);
    string elementTypeId = fileSetView.paramAt(2);
    string fileTypeId = fileSetView.paramAt(3);
    string dataRep = fileSetView.paramAt(4);
    DataType* fileType = getDataTypeById(fileTypeId);
    assert("NATIVE" == dataRep);
    assert(0 != fileType);

    // Set the file view for this descriptor
    FileView fileView(displacement, fileType->clone());
    fd->setFileView(fileView);
}

void PHTFIOApplication::performTypeContiguous(const PHTFEventRecord& typeContiguous)
{
    // Create the data type
    size_t count = typeContiguous.paramAsSizeT(0);
    string oldTypeId = typeContiguous.paramAt(1);
    DataType* oldType = getDataTypeById(oldTypeId);
    ContiguousDataType* dataType = new ContiguousDataType(count, *oldType);

    // Register the data type
    string newTypeId = typeContiguous.paramAt(2);
    dataTypeById_[newTypeId] = dataType;
}

void PHTFIOApplication::performTypeCreateSubarray(const PHTFEventRecord& createSubarray)
{
    // Create the data type
    size_t ndims = createSubarray.paramAsSizeT(0);
    vector<size_t> sizes = createSubarray.paramAsVector(1);
    vector<size_t> subSizes = createSubarray.paramAsVector(2);
    vector<size_t> starts = createSubarray.paramAsVector(3);
    int order = createSubarray.paramAsSizeT(4);
    string oldTypeId = createSubarray.paramAt(5);
    DataType* oldType = getDataTypeById(oldTypeId);
    SubarrayDataType* dataType = new SubarrayDataType(sizes,
                                                      subSizes,
                                                      starts,
                                                      SubarrayDataType::C_ORDER,
                                                      *oldType);
    assert(56 == order);
    assert (ndims == sizes.size());

    // Register the new data type
    string newTypeId = createSubarray.paramAt(6);
    dataTypeById_[newTypeId] = dataType;

    //cerr << "Created new type: " << *dataType << endl;
}

spfsMPIBarrierRequest* PHTFIOApplication::createAllReduceMessage(
    const PHTFEventRecord* allReduceRecord)
{
    Communicator communicatorId = allReduceRecord->paramAsAddress(5);

    spfsMPIBarrierRequest* barrier =
        new spfsMPIBarrierRequest(0, SPFS_MPI_BARRIER_REQUEST);
    barrier->setCommunicator(communicatorId);
    return barrier;
}

spfsMPIBarrierRequest* PHTFIOApplication::createBarrierMessage(
    const PHTFEventRecord* barrierRecord)
{
    Communicator communicatorId = barrierRecord->paramAsAddress(0);

    spfsMPIBarrierRequest* barrier =
        new spfsMPIBarrierRequest(0, SPFS_MPI_BARRIER_REQUEST);
    barrier->setCommunicator(communicatorId);
    return barrier;
}

spfsMPIBcastRequest* PHTFIOApplication::createBcastMessage(
    const PHTFEventRecord* bcastRecord)
{
    Communicator communicatorId = bcastRecord->paramAsAddress(4);

    spfsMPIBcastRequest* bcast =
        new spfsMPIBcastRequest(0, SPFS_MPI_BCAST_REQUEST);
    bcast->setCommunicator(communicatorId);
    return bcast;
}

cMessage* PHTFIOApplication::createCPUPhaseMessage(
    const PHTFEventRecord* cpuRecord)
{
    double delay = cpuRecord->duration();

    cMessage *msg = new cMessage(CPU_PHASE_MESSAGE_NAME);
    cPar *cp = new cPar("Delay");
    *cp = delay;
    msg->addPar(cp);
    return msg;
}

spfsMPIFileCloseRequest* PHTFIOApplication::createFileCloseMessage(
    const PHTFEventRecord* closeRecord)
{
    // Retrieve the descriptor
    int handle = closeRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);
    assert(0 != fd);

    spfsMPIFileCloseRequest* close = new spfsMPIFileCloseRequest(
        0, SPFS_MPI_FILE_CLOSE_REQUEST);
    close->setFileDes(fd);
    return close;
}

spfsMPIFileDeleteRequest* PHTFIOApplication::createFileDeleteMessage(
    const PHTFEventRecord* deleteRecord)
{
    // Extract the file name
    string filename = deleteRecord->paramAt(0);
    cerr << "DIAGNOSTIC: Deleting Filename: " << filename << endl;

    // Fill out the delete request
    spfsMPIFileDeleteRequest* deleteRequest =
        new spfsMPIFileDeleteRequest(0, SPFS_MPI_FILE_DELETE_REQUEST);
    deleteRequest->setFileName(filename.c_str());
    return deleteRequest;
}

spfsMPIFileStatRequest* PHTFIOApplication::createFileGetInfoMessage(
    const PHTFEventRecord* getSizeRecord)
{
    // Retrieve the descriptor
    int handle = getSizeRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    spfsMPIFileStatRequest* stat = new spfsMPIFileStatRequest(
        0, SPFS_MPI_FILE_STAT_REQUEST);
    stat->setFileName(fd->getFilename().c_str());
    stat->setDetermineFileSize(false);
    return stat;
}

spfsMPIFileStatRequest* PHTFIOApplication::createFileGetSizeMessage(
    const PHTFEventRecord* getSizeRecord)
{
    // Retrieve the descriptor
    int handle = getSizeRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    spfsMPIFileStatRequest* stat = new spfsMPIFileStatRequest(
        0, SPFS_MPI_FILE_STAT_REQUEST);
    stat->setFileName(fd->getFilename().c_str());
    stat->setDetermineFileSize(true);
    return stat;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createFileIReadMessage(
    const PHTFEventRecord* readRecord)
{
    uint64_t requestId = readRecord->paramAsAddress(4);

    spfsMPIFileReadAtRequest* iread = createFileReadMessage(readRecord);
    iread->setReqId(requestId);

    // Update the list of non-blocking operations still pending
    pendingRequestsById_[requestId] = iread;
    return iread;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createFileIWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    uint64_t requestId = writeRecord->paramAsAddress(4);

    spfsMPIFileWriteAtRequest* iwrite = createFileWriteMessage(writeRecord);
    iwrite->setReqId(requestId);

    // Update the list of non-blocking operations still pending
    pendingRequestsById_[requestId] = iwrite;
    return iwrite;
}

spfsMPIFileOpenRequest* PHTFIOApplication::createFileOpenMessage(
    const PHTFEventRecord* openRecord)
{
    // Extract the communicator id
    Communicator communicatorId = openRecord->paramAsAddress(0);

    // Extract the open mode
    int mode = openRecord->paramAsSizeT(2);

    // Extract the descriptor id
    int fileId = openRecord->paramAsDescriptor(4, *phtfEvent_);
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Open file id: " << fileId << endl;

    // Retrieve the descriptor
    FileDescriptor* fd = getDescriptor(fileId);
    assert(0 != fd);

    // Fill out the open request
    spfsMPIFileOpenRequest* open = new spfsMPIFileOpenRequest(
        0, SPFS_MPI_FILE_OPEN_REQUEST);
    open->setCommunicator(communicatorId);
    open->setFileName(fd->getFilename().c_str());
    open->setFileDes(fd);
    open->setMode(mode);
    return open;
}


spfsMPIFileReadAtRequest* PHTFIOApplication::createFileReadAtMessage(
    const PHTFEventRecord* readAtRecord)
{
    int handle = readAtRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    size_t offset = readAtRecord->paramAsSizeT(1);
    size_t count = readAtRecord->paramAsSizeT(3);
    string dtId = readAtRecord->paramAt(4);
    DataType* dataType = getDataTypeById(dtId);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(count);
    read->setDataType(dataType->clone());
    read->setOffset(offset);
    read->setFileDes(fd);
    read->setReqId(-1);
    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createFileReadAllMessage(
    const PHTFEventRecord* readRecord)
{
    spfsMPIFileReadAtRequest* read = createFileReadMessage(readRecord);

    // Set the collective attributes
    int handle = readRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);
    Communicator comm = fd->getCommunicator();
    read->setIsCollective(true);
    read->setCommunicator(comm);
    read->setRank(CommMan::instance().commRank(comm, getRank()));

    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createFileReadMessage(
    const PHTFEventRecord* readRecord)
{
    int handle = readRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    size_t count = readRecord->paramAsSizeT(2);

    string dtId = readRecord->paramAt(3);
    DataType* dataType = getDataTypeById(dtId);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(count);
    read->setDataType(dataType->clone());
    read->setFileDes(fd);
    read->setOffset(fd->getFilePointer());
    read->setReqId(-1);

    // Increment the file pointer
    size_t incDistance = count * dataType->getExtent();
    fd->moveFilePointer(incDistance);

    return read;
}

spfsMPIFileStatRequest* PHTFIOApplication::createFileSetSizeMessage(
    const PHTFEventRecord* setSizeRecord)
{
    // Retrieve the descriptor
    int handle = setSizeRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    spfsMPIFileStatRequest* stat = new spfsMPIFileStatRequest(
        0, SPFS_MPI_FILE_STAT_REQUEST);
    stat->setFileName(fd->getFilename().c_str());
    stat->setDetermineFileSize(false);
    return stat;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createFileWriteAtMessage(
    const PHTFEventRecord* writeAtRecord)
{
    int handle = writeAtRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    size_t offset = writeAtRecord->paramAsSizeT(1);
    size_t count = writeAtRecord->paramAsSizeT(3);

    string dtId = writeAtRecord->paramAt(4);
    DataType* dataType = getDataTypeById(dtId);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setCount(count);
    write->setDataType(dataType->clone());
    write->setOffset(offset);
    write->setReqId(-1);
    write->setFileDes(fd);

    return write;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createFileWriteAllMessage(
    const PHTFEventRecord* writeRecord)
{
    spfsMPIFileWriteAtRequest* write = createFileWriteMessage(writeRecord);

    // Set the collective attributes
    int handle = writeRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);
    Communicator comm = fd->getCommunicator();
    write->setIsCollective(true);
    write->setCommunicator(comm);
    write->setRank(CommMan::instance().commRank(comm, getRank()));

    return write;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createFileWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    int handle = writeRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    size_t count = writeRecord->paramAsSizeT(2);

    string dtId = writeRecord->paramAt(3);
    DataType* dataType = getDataTypeById(dtId);
    assert(NULL != dataType);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setDataType(dataType->clone());
    write->setCount(count);
    write->setOffset(fd->getFilePointer());
    write->setReqId(-1);
    write->setFileDes(fd);

    // Increment the file pointer
    size_t incDistance = count * dataType->getExtent();
    fd->moveFilePointer(incDistance);

    return write;
}

spfsMPIBcastRequest* PHTFIOApplication::createBcastRequest(
    Communicator communicatorId)
{
    assert(CommMan::instance().exists(communicatorId));

    spfsMPIBcastRequest* bcast =
        new spfsMPIBcastRequest(0, SPFS_MPI_BCAST_REQUEST);
    bcast->setCommunicator(communicatorId);
    return bcast;
}

DataType* PHTFIOApplication::getDataTypeById(const string& typeId)
{
    DataType* dataType = 0;
    DataTypeMap::const_iterator iter = dataTypeById_.find(typeId);
    if (iter == dataTypeById_.end())
    {
        // No data type in map, so look it up in the configuration
        string sizeString = PHTFTrace::instance().getFs()->consts(typeId);
        istringstream iss(sizeString);
        size_t typeWidth = 0;
        iss >> typeWidth;
        switch(typeWidth)
        {
            case 1:
                dataType = new BasicDataType<1>();
                break;
            case 2:
                dataType = new BasicDataType<2>();
                break;
            case 4:
                dataType = new BasicDataType<4>();
                break;
            case 8:
                dataType = new BasicDataType<8>();
                break;
            default:
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "Error: unsupported basic type: width " << typeWidth << endl;
                assert(false);
                break;
        }
        dataTypeById_[typeId] = dataType;
    }
    else
    {
        dataType = iter->second;
    }
    assert(0 != dataType);
    return dataType;
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
