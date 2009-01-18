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

    // Retrieve the resource describing the trace location
    traceDirectory_ = par("traceFile").stringValue();

    // PHTF stuff not able to support single initialization, so
    // enforce that locally
    static bool phtfInit = false;
    if(!phtfInit)
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

/**
 * Cleanup trace and tally statistics
 */
void PHTFIOApplication::finish()
{
    IOApplication::finish();

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
        cerr << "Aggregated Write bandwidth: " << aggWriteBandwidth << endl;

        double aggReadBandwidth = aggReadBytes / (1.0e6 * maxReadTime);
        recordScalar("SPFS Aggregate Read Bandwidth", aggReadBandwidth);
        cerr << "Aggregated Read bandwidth: " << aggReadBandwidth << endl;
    }

    // Cleanup trace resources
    phtfEvent_->close();
}

void PHTFIOApplication::rankChanged(int oldRank)
{
    // Ensure we only join the communicator once
    assert(-1 == oldRank);

    // Join the world communicator on rank initialization
    CommMan::instance().registerRank(getRank());
}


void PHTFIOApplication::populateFileSystem()
{
    cerr << "Populating file system . . . ";
    PHTFFs *fs = PHTFTrace::instance().getFs();
    FileSystemMap fsm;
    for(int i = 0; i < fs->fileNum(); i ++)
    {
        fsm[fs->fileName(i)] = fs->fileSize(i);
    }
    FileBuilder::instance().populateFileSystem(fsm);
    cerr << "Done." << endl;
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
        int commId = openRequest->getCommunicator();

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

        // TODO: Use this to look at the actual events
        //cerr << "[" << getRank() << "] " << eventRecord.recordStr() << endl;
        rec_id_ = eventRecord.recordId();
        msgScheduled = processEvent(eventRecord);
    }
    return msgScheduled;
}

bool PHTFIOApplication::processEvent(PHTFEventRecord& eventRecord)
{
    bool msgScheduled = false;
    int opType = eventRecord.recordOp();
    if (CPU_PHASE == opType || OPEN == opType ||
        IREAD == opType || IWRITE == opType || WAIT == opType ||
        COMM_DUP == opType || COMM_RANK == opType)
    {
        msgScheduled = processIrregularEvent(&eventRecord);
    }
    else if (ALLREDUCE == opType || BARRIER == opType || BCAST == opType)
    {
        cMessage* request = createMessage(&eventRecord);
        send(request, mpiOutGate_);
        msgScheduled = true;
    }
    else
    {
        cMessage* request = createMessage(&eventRecord);
        send(request, ioOutGate_);
        msgScheduled = true;
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
            cMessage* msg = createCPUPhaseMessage(eventRecord);
            scheduleCPUMessage(msg);
            msgScheduled = true;
            break;
        }
        case COMM_CREATE:
        case COMM_DUP:
        //case COMM_JOIN:
        case COMM_RANK:
        //case COMM_SIZE:
        case COMM_SPLIT:
        {
            // Perform communicator create operation
            performCommProcessing(eventRecord);

            // Retrieve the next operation, as this one is local
            msgScheduled = scheduleNextMessage();
            break;
        }
        case OPEN:
        {
            // Perform the open processing
            uint64_t commId = 0;
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
            performSetViewProcessing(eventRecord);
            msgScheduled = scheduleNextMessage();
            break;
        }
        case TYPE_CONTIGUOUS:
        case TYPE_STRUCT:
        case TYPE_CREATE_SUBARRAY:
        case TYPE_VECTOR:
        {
            // Perform type_contiguous operation
            performTypeProcessing(eventRecord);
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
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Unable to handle irregular event: "
                 << eventRecord->recordStr() << endl;
            assert(0);
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
        case READ_ALL:
        {
            request = createFileReadMessage(eventRecord);
            break;
        }
        case READ_AT:
        case READ_AT_ALL:
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
        case WRITE_ALL:
        {
            request = createFileWriteMessage(eventRecord);
            break;
        }
        case WRITE_AT:
        case WRITE_AT_ALL:
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
    cModule* jobProcess = mpiProcess->parentModule();
    assert(0 != jobProcess);
    cModule* cacheModule = jobProcess->submodule("cache");
    assert(0 != cacheModule);
    MiddlewareCache* middlewareCache = dynamic_cast<MiddlewareCache*>(cacheModule);
    assert(0 != middlewareCache);
    middlewareCache->performFakeOpen(fd->getFilename());
}

void PHTFIOApplication::performOpenProcessing(PHTFEventRecord* openRecord,
                                              uint64_t& outCommunicatorId)
{
    // Extract the filename
    Filename openFilename(openRecord->paramAt(1));

    // Extract the descriptor number from the event record
    int fileId = openRecord->paramAsDescriptor(4, *phtfEvent_);

    // Construct a file descriptor for use in simulaiton
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(openFilename);
    assert(0 != fd);

    // Associate the file id with a file descriptor
    setDescriptor(fileId, fd);

    // Extract the communicator id
    outCommunicatorId = openRecord->paramAsAddress(0);
}

void PHTFIOApplication::performSeekProcessing(PHTFEventRecord* seekRecord)
{
    uint64_t handle = seekRecord->paramAsAddress(0);

    size_t offset = seekRecord->paramAsSizeT(1);
    int whence = seekRecord->paramAsSizeT(2);
    FileDescriptor* fd = getDescriptor(handle);

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

void PHTFIOApplication::performSetViewProcessing(PHTFEventRecord* setViewRecord)
{
    uint64_t handle = setViewRecord->paramAsAddress(0);
    FileDescriptor* fd = getDescriptor(handle);

    size_t offset = setViewRecord->paramAsSizeT(1);

    string dtstr = setViewRecord->paramAt(3);
    dtstr = getAlias(dtstr);
    DataType* dataType = getDataTypeById(dtstr);
    assert(dataType);

    FileView fileView(offset, dataType->clone());
    fd->setFileView(fileView);
}

void PHTFIOApplication::performCommProcessing(PHTFEventRecord* commRecord)
{
    int op = commRecord->recordOp();
    switch(op)
    {
        case COMM_DUP:
        {
            Communicator oldComm = commRecord->paramAsAddress(0);
            Communicator newComm = commRecord->paramAsAddress(1);

            performDuplicateCommunicator(oldComm, newComm);
            break;
        }
        case COMM_CREATE:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Comm Create requires group support." << endl;
            assert(false);
            //string newcomm comm;
            //performCreateCommunicator(newcomm);
            break;
        }
        case COMM_SPLIT:
        {
            assert(false);
            string newcomm = commRecord->paramAt(3);
            break;
        }
        case COMM_RANK:
        {
            // TODO: It should be possible to make sure the rank matches
            // the communicator rank here
            //Communicator comm = commRecord->paramAsAddress(0);
            //assert(CommMan::instance().exists(comm));
            break;
        }
        default:
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "Error: unsupported comm op type: " << op << endl;
            assert(false);
            break;
        }
    }
}

void PHTFIOApplication::performCreateCommunicator(string newcomm)
{
    cerr << __FILE__ << ":" << __LINE__ << ":Create comm " << newcomm;
    string aliasComm = getAlias(newcomm);
    Communicator comm = (Communicator)strtol(aliasComm.c_str(), NULL, 0);

    cerr << " alias "  << comm;

    // create only if communicator not yet exist
    if(!CommMan::instance().exists(comm))
    {
        // if this is a new communicator, create it
        if(phtfEvent_->memValue(aliasComm, "ranks").compare(""))
        {
            string ssize = phtfEvent_->memValue(aliasComm, "size");
            string srank;
            int commsize = (int)strtol(ssize.c_str(), NULL, 0);
            int commrank;
            stringstream ss("");
            ss << phtfEvent_->memValue(aliasComm, "ranks");

            cerr << " add " << ss.str();
            for(int i = 0; i < commsize; i ++)
            {
                ss >> srank;
                commrank = (int)strtol(srank.c_str(), NULL, 0);
                CommMan::instance().joinComm(comm, commrank);
            }
        }
        else
        {
            cerr << "Error: don't know how to create comm " << aliasComm << endl;
            assert(false);
        }
    }
    cerr << ": " << CommMan::instance().commSize(comm) << endl;
}

void PHTFIOApplication::performDuplicateCommunicator(const Communicator& oldComm,
                                                     Communicator& newComm)
{
    assert(true == CommMan::instance().exists(oldComm));
    CommMan::instance().dupComm(oldComm, newComm);
}

void PHTFIOApplication::performTypeProcessing(PHTFEventRecord* typeRecord)
{
    // get type id
    string typeId = typeRecord->paramAt(typeRecord->paraNum() - 1);

    // create type
    performCreateDataType(typeId);
}

void PHTFIOApplication::performCreateDataType(string typeId)
{
    // create type based on datatype type

    stringstream ss("");
    ss << typeId << "@" << rec_id_;


    string realtype = phtfEvent_->memValue("Pointer", ss.str());
    stringstream ss2("");
    ss2 << realtype << "@" << rec_id_;

    cerr << __FILE__ << ":" << __LINE__ << ":" << "Create datatype " << realtype << " ";

    int type = (int)strtol(phtfEvent_->memValue(ss2.str(), "type").c_str(), NULL, 0);
    switch(type)
    {
        case BASIC:
            performCreateBasicDataType(realtype);
            break;
        case CONTIGUOUS:
            performCreateContiguousDataType(realtype);
            break;
        case STRUCT:
            performCreateStructDataType(realtype);
            break;
        case SUBARRAY:
            performCreateSubarrayDataType(realtype);
            break;
        case VECTOR:
            performCreateVectorDataType(realtype);
            break;
        default:
            cerr << "Error: unsupported datatype: " << type << endl;
            assert(false);
            break;
    }
}

DataType * PHTFIOApplication::getDataTypeById(std::string typeId)
{
    stringstream ss("");
    ss << typeId << "@" << rec_id_;

    if(!dataTypeById_.count(typeId))
    {
        if(!dataTypeById_.count(ss.str()))
        {
            performCreateBasicDataType(typeId);
            if(!dataTypeById_.count(typeId))
            {
                return NULL;
            }
            else
            {
                return dataTypeById_[typeId];
            }
        }
        else
        {
            return dataTypeById_[ss.str()];
        }
    }
    else
    {
        return dataTypeById_[typeId];
    }
}

void PHTFIOApplication::performCreateBasicDataType(std::string typeId)
{
    // get parameter
    size_t size;
    string ssize = PHTFTrace::instance().getFs()->consts(typeId);
    if(!ssize.compare(""))
    {
        ssize = phtfEvent_->memValue(typeId, "size");
        if(!ssize.compare(""))
        {
            ssize = typeId;
        }
    }

    size = (size_t)strtol(ssize.c_str(), 0, 0);

    DataType * newDataType;
    // create newtype
    switch(size)
    {
        case 1:
            newDataType = new BasicDataType<1>();
            break;
        case 2:
            newDataType = new BasicDataType<2>();
            break;
        case 4:
            newDataType = new BasicDataType<4>();
            break;
        case 8:
            newDataType = new BasicDataType<8>();
            break;
        default:
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "Error: unsupported basic type: width " << size << endl;
            assert(false);
            break;
    }

    dataTypeById_[typeId] = newDataType;
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Data Type Width: " << dataTypeById_.size() << endl;
}

void PHTFIOApplication::performCreateContiguousDataType(std::string typeId)
{
    stringstream ss("");
    ss << typeId << "@" << rec_id_;

    // get parameter
    size_t count = (size_t)strtol(phtfEvent_->memValue(ss.str(), "count").c_str(), NULL, 0);
    string oldTypeId = phtfEvent_->memValue(ss.str(), "oldtype");

    // create oldtype if not exist
    DataType * oldType = getDataTypeById(oldTypeId);
    if(!oldType)
    {
        performCreateDataType(oldTypeId);
        oldType = getDataTypeById(oldTypeId);
    }

    assert(oldType);

    // create newtype
    ContiguousDataType * newDataType = new ContiguousDataType(count, *oldType);
    dataTypeById_[typeId] = newDataType;
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Data Type Width: " << dataTypeById_.size() << endl;
}

void PHTFIOApplication::performCreateStructDataType(std::string typeId)
{
    stringstream ss("");
    ss << typeId << "@" << rec_id_;

    // get parameter
    size_t count = (size_t)strtol(phtfEvent_->memValue(ss.str(), "count").c_str(), NULL, 0);
    stringstream ssOldTypes("");
    stringstream ssDisp("");
    stringstream ssBlock("");
    ssOldTypes << phtfEvent_->memValue(ss.str(), "oldtypes");
    ssDisp << phtfEvent_->memValue(ss.str(), "indices");
    ssBlock << phtfEvent_->memValue(ss.str(), "blocklens");

    vector<size_t> blocklens;
    vector<size_t> disp;
    vector<const DataType*> oldTypes;

    for(size_t i = 0; i < count; i ++)
    {
        string sBlock;
        ssBlock >> sBlock;
        size_t b = (size_t)strtol(sBlock.c_str(), NULL, 0);
        blocklens.push_back(b);

        string sDisp;
        ssDisp >> sDisp;
        size_t d = (size_t)strtol(sDisp.c_str(), NULL, 0);
        disp.push_back(d);

        string oldTypeId;
        ssOldTypes >> oldTypeId;

        // create old type if not exist
        DataType *oldType = getDataTypeById(oldTypeId);
        if(!oldType)
        {
            performCreateDataType(oldTypeId);
            oldType = getDataTypeById(oldTypeId);
        }
        assert(oldType);
        oldTypes.push_back(oldType);
    }

    // create new type
    StructDataType * newDataType = new StructDataType(blocklens, disp, oldTypes);
    dataTypeById_[typeId] = newDataType;
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Data Type Width: " << dataTypeById_.size() << endl;
}

void PHTFIOApplication::performCreateSubarrayDataType(std::string typeId)
{
    stringstream ss("");
    ss << typeId << "@" << rec_id_;

    // get parameter
    size_t ndims = (size_t)strtol(phtfEvent_->memValue(ss.str(), "ndims").c_str(), NULL, 0);
    string oldTypeId = phtfEvent_->memValue(ss.str(), "oldtype");
    int order = (int)strtol(phtfEvent_->memValue(ss.str(), "order").c_str(), NULL, 0);
    stringstream ssSizes("");
    stringstream ssStarts("");
    stringstream ssSubSizes("");

    ssSizes << phtfEvent_->memValue(ss.str(), "sizes");
    ssStarts << phtfEvent_->memValue(ss.str(), "starts");
    ssSubSizes << phtfEvent_->memValue(ss.str(), "subsizes");

    vector<size_t> sizes;
    vector<size_t> subSizes;
    vector<size_t> starts;

    for(size_t i = 0; i < ndims; i ++)
    {
        string sSize;
        ssSizes >> sSize;
        size_t s = (size_t)strtol(sSize.c_str(), NULL, 0);
        sizes.push_back(s);

        string sSubSize;
        ssSubSizes >> sSubSize;
        size_t sb = (size_t)strtol(sSubSize.c_str(), NULL, 0);
        subSizes.push_back(sb);

        string sStart;
        ssStarts >> sStart;
        size_t st = (size_t)strtol(sStart.c_str(), NULL, 0);
        starts.push_back(st);
    }

    // create old type if not exist
    DataType * oldType = getDataTypeById(oldTypeId);
    if(!oldType)
    {
        performCreateDataType(oldTypeId);
        oldType = getDataTypeById(oldTypeId);
    }
    assert(oldType);

    SubarrayDataType * newDataType;
    // create new type
    if(order == 57)
        newDataType = new SubarrayDataType(sizes, subSizes, starts, SubarrayDataType::FORTRAN_ORDER, *oldType);
    else
        newDataType = new SubarrayDataType(sizes, subSizes, starts, SubarrayDataType::C_ORDER, *oldType);
    dataTypeById_[typeId] = newDataType;
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Data Type Width: " << dataTypeById_.size() << endl;
}

void PHTFIOApplication::performCreateVectorDataType(std::string typeId)
{
    stringstream ss("");
    ss << typeId << "@" << rec_id_;

    // get parameter
    size_t blocklength = (size_t)strtol(phtfEvent_->memValue(ss.str(), "blocklength").c_str(), NULL, 0);
    size_t count = (size_t)strtol(phtfEvent_->memValue(ss.str(), "count").c_str(), NULL, 0);
    string oldTypeId = phtfEvent_->memValue(ss.str(), "oldtype");
    int stride = (int)strtol(phtfEvent_->memValue(ss.str(), "stride").c_str(), NULL, 0);

    // create old type if not exist
    DataType * oldType = getDataTypeById(oldTypeId);
    if(!oldType)
    {
        performCreateDataType(oldTypeId);
        oldType = getDataTypeById(oldTypeId);
    }
    assert(oldType);

    VectorDataType * newDataType = new VectorDataType(count, blocklength, stride, *oldType);
    dataTypeById_[typeId] = newDataType;
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Data Type Width: " << dataTypeById_.size() << endl;
}

spfsMPIBarrierRequest* PHTFIOApplication::createAllReduceMessage(
    const PHTFEventRecord* allReduceRecord)
{
    uint64_t communicatorId = allReduceRecord->paramAsAddress(5);

    spfsMPIBarrierRequest* barrier =
        new spfsMPIBarrierRequest(0, SPFS_MPI_BARRIER_REQUEST);
    barrier->setCommunicator(communicatorId);
    return barrier;
}

spfsMPIBarrierRequest* PHTFIOApplication::createBarrierMessage(
    const PHTFEventRecord* barrierRecord)
{
    uint64_t communicatorId = barrierRecord->paramAsAddress(0);

    spfsMPIBarrierRequest* barrier =
        new spfsMPIBarrierRequest(0, SPFS_MPI_BARRIER_REQUEST);
    barrier->setCommunicator(communicatorId);
    return barrier;
}

spfsMPIBcastRequest* PHTFIOApplication::createBcastMessage(
    const PHTFEventRecord* bcastRecord)
{
    uint64_t communicatorId = bcastRecord->paramAsAddress(4);

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
    string filename = deleteRecord->paramAsFilename(0, *phtfEvent_);

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
    string gstr = openRecord->paramAt(0);
    gstr = getAlias(gstr);
    int communicatorId = (int)strtol(gstr.c_str(), NULL, 0);

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
    string dtstr = readAtRecord->paramAt(4);
    dtstr = getAlias(dtstr);
    DataType* dataType = getDataTypeById(dtstr);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(count);
    read->setDataType(dataType->clone());
    read->setOffset(offset);
    read->setFileDes(fd);
    read->setReqId(-1);
    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createFileReadMessage(
    const PHTFEventRecord* readRecord)
{
    int handle = readRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    size_t count = readRecord->paramAsSizeT(2);

    string dtstr = readRecord->paramAt(3);
    dtstr = getAlias(dtstr);
    DataType* dataType = getDataTypeById(dtstr);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(count);
    read->setDataType(dataType->clone());
    read->setFileDes(fd);
    read->setOffset(fd->getFilePointer());
    read->setReqId(-1);

    // Increment the file pointer
    fd->moveFilePointer(count * dataType->getExtent());

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

    string dtstr = writeAtRecord->paramAt(4);
    dtstr = getAlias(dtstr);
    DataType* dataType = getDataTypeById(dtstr);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setCount(count);
    write->setDataType(dataType->clone());
    write->setOffset(offset);
    write->setReqId(-1);
    write->setFileDes(fd);

    return write;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createFileWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    int handle = writeRecord->paramAsDescriptor(0, *phtfEvent_);
    FileDescriptor* fd = getDescriptor(handle);

    size_t count = writeRecord->paramAsSizeT(2);

    string dtstr = writeRecord->paramAt(3);
    dtstr = getAlias(dtstr);
    DataType* dataType = getDataTypeById(dtstr);
    assert(NULL != dataType);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setDataType(dataType->clone());
    write->setCount(count);
    write->setOffset(fd->getFilePointer());
    write->setReqId(-1);
    write->setFileDes(fd);

    // Increment the file pointer
    fd->moveFilePointer(count * dataType->getExtent());

    return write;
}

string PHTFIOApplication::getAlias(string id)
{
    string alias = phtfEvent_->memValue("Alias", id);
    if(alias.compare(""))
        return getAlias(alias);
    else
    {
        if(id.length() >= 8)
        {
            stringstream ss("");
            ss << "0x" << id.substr(id.length() - 8);
            alias = phtfEvent_->memValue("Alias", ss.str());
            if(alias.compare(""))
            {
                return getAlias(alias);
            }
            else
            {
                return id;
            }
        }
        else
        {
            return id;
        }
    }
}

spfsMPIBcastRequest* PHTFIOApplication::createBcastRequest(
    int communicatorId)
{
    assert(-1 != communicatorId);

    spfsMPIBcastRequest* bcast =
        new spfsMPIBcastRequest(0, SPFS_MPI_BCAST_REQUEST);
    bcast->setCommunicator(communicatorId);
    return bcast;
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
