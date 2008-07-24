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
#include "mpi_proto_m.h"
#include "storage_layout_manager.h"
#include "phtf_io_trace.h"
#include "comm_man.h"

using namespace std;

// OMNet Registriation Method
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
    // Parent class initilization
    IOApplication::initialize();

    // Retrieve the resource describing the trace location
    string dirStr = par("dirPHTF").stringValue();

    // PHTF stuff not able to support single initialization, so
    // enforce that locally
    static bool phtfInit = false;
    if(!phtfInit)
    {
        // Set value for MPI_COMM_WORLD & MPI_COMM_SELF based on configuration
        // in fs.ini
        string commWorldValue =
            PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_COMM_WORLD");
        CommMan::instance().setCommWorld(strtol(commWorldValue.c_str(), 0, 0));

        string commSelfValue =
            PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_COMM_SELF");
        CommMan::instance().setCommSelf(strtol(commSelfValue.c_str(), 0, 0));

        CommMan::instance().joinComm(SPFS_COMM_WORLD, getRank());

        // Build a singleton map of mpi operation IDs (string => ID)
        PHTFEventRecord::buildOpMap();

        // Disable further initialization
        phtfInit = true;
    }

    // Schedule the kick start message
    cMessage* kickStart = new cMessage();
    scheduleAt(0.0, kickStart);
}

/**
 * Cleanup trace and tally statistics
 */
void PHTFIOApplication::finish()
{
    IOApplication::finish();

    phtfEvent_->close();
}

void PHTFIOApplication::handleMessage(cMessage* msg)
{
    // Add code to allow opens to finish by broadcasting result
    if (SPFS_MPI_FILE_OPEN_RESPONSE == msg->kind())
    {
        // Extract the communicator from the open request
        spfsMPIFileOpenRequest* openRequest =
            static_cast<spfsMPIFileOpenRequest*>(msg->contextPointer());
        assert(0 != openRequest);

        // Broadcast result to the remainder of the communicator
        int commId = openRequest->getCommunicator();
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

bool PHTFIOApplication::scheduleNextMessage()
{
    if(!phtfEvent_)
    {
        string dirStr = par("dirPHTF").stringValue();
        // get event file for this ioapp
        phtfEvent_ = PHTFTrace::getInstance(dirStr)->getEvent(getRank());
        phtfEvent_->open();
    }

    bool msgScheduled = false;
    if (!phtfEvent_->eof())
    {
        PHTFEventRecord eventRecord;
        *phtfEvent_ >> eventRecord;

        cout << eventRecord.recordStr() << endl;

        rec_id_ = eventRecord.recordId();

        int opcode = eventRecord.recordOp();
        if (CPU_PHASE == opcode)
        {
            cMessage* msg = createCPUPhaseMessage(&eventRecord);
            scheduleCPUMessage(msg);
            msgScheduled = true;
        }
        else if (GET_INFO == opcode)
        {
            // Currently exists as a no-op
            msgScheduled = scheduleNextMessage();
        }
        else if (WAIT == opcode)
        {
            // Perform wait operation
            bool waitIsComplete = false;
            performWaitProcessing(&eventRecord, waitIsComplete);

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
        }
        else if (SEEK == opcode)
        {
            // Perform seek operation
            performSeekProcessing(&eventRecord);

            // Retrieve the next operation, as this one is local
            msgScheduled = scheduleNextMessage();
        }
        else if (OPEN == opcode)
        {
            // Perform the open processing
            int commId = -1;
            performOpenProcessing(&eventRecord, commId);
            assert(commId != -1);

            // Determine if this is first rank for the communicator
            int openRank = CommMan::instance().commRank(commId, getRank());
            if (0 == openRank)
            {
                cMessage* msg = createRequest(&eventRecord);
                send(msg, ioOutGate_);
            }
            else
            {
                cMessage* msg = createBcastRequest(commId);
                send(msg, mpiOutGate_);
            }
            msgScheduled = true;
        }
        else if (BARRIER == opcode)
        {
            cMessage* msg = createRequest(&eventRecord);
            send(msg, mpiOutGate_);
            msgScheduled = true;
        }
        else if (TYPE_CONTIGUOUS == opcode || TYPE_STRUCT == opcode || TYPE_CREATE_SUBARRAY == opcode || TYPE_VECTOR == opcode)
        {
            // Perform type_contiguous operation
            performTypeProcessing(&eventRecord);

            // Retrieve the next operation, as this one is local
            msgScheduled = scheduleNextMessage();
        }
        else if (COMM_DUP == opcode || COMM_CREATE == opcode || COMM_SPLIT == opcode || COMM_RANK == opcode)
        {
            // Perform communicator create operation
            performCommProcessing(&eventRecord);

            // Retrieve the next operation, as this one is local
            msgScheduled = scheduleNextMessage();
        }
        else if (SET_VIEW == opcode)
        {
            performSetViewProcessing(&eventRecord);
            msgScheduled = scheduleNextMessage();
        }
        else if (GET_SIZE == opcode || SET_SIZE == opcode)
        {
            msgScheduled = scheduleNextMessage();
        }
        else
        {
            // Start the non-blocking operation
            cMessage* msg = createRequest(&eventRecord);
            send(msg, ioOutGate_);

            // Handle non-blocking operations by scheduling another request
            if (IREAD == opcode || IWRITE == opcode)
            {
                msgScheduled = scheduleNextMessage();
            }
            else
            {
                msgScheduled = true;
            }
        }
    }
    return msgScheduled;
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

void PHTFIOApplication::scheduleCPUMessage(cMessage *msg)
{
    double schTime = simTime() + msg->par("Delay").doubleValue();
    scheduleAt(schTime , msg);
}

void PHTFIOApplication::rankChanged(int oldRank)
{
    // Ensure we only join the communicator once
    assert(-1 == oldRank);

    // Join the world communicator on rank initialization
    CommMan::instance().registerRank(getRank());
}

spfsMPIRequest* PHTFIOApplication::createRequest(PHTFEventRecord* rec)
{
    assert(0 != rec);
    assert(rec->recordOp() != CPU_PHASE);
    assert(rec->recordOp() != SEEK);
    assert(rec->recordOp() != WAIT);
    spfsMPIRequest* mpiMsg = 0;

    switch(rec->recordOp())
    {
        case BARRIER:
            mpiMsg = createBarrierMessage(rec);
            break;
        case OPEN:
            mpiMsg = createOpenMessage(rec);
            break;
        case CLOSE:
            mpiMsg = createCloseMessage(rec);
            break;
        case DELETE:
        {
            mpiMsg = createDeleteRequest(rec);
            break;
        }
        case READ_AT:
        case READ_AT_ALL:
            mpiMsg = createReadAtMessage(rec);
            break;
        case WRITE_AT:
        case WRITE_AT_ALL:
            mpiMsg = createWriteAtMessage(rec);
            break;
        case IREAD:
            mpiMsg = createIReadMessage(rec);
            break;
        case IWRITE:
            mpiMsg = createIWriteMessage(rec);
            break;
        default:
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "ERROR: Invalid PHTF Record Op: " << rec->recordOp()
                 << " -> " << rec->recordStr()
                 << endl;
            assert(false);
            break;
    }

    return mpiMsg;
}

void PHTFIOApplication::populateFileSystem()
{
    cerr << "Populating file system . . . ";

    string dirStr = par("dirPHTF").stringValue();
    PHTFFs *fs = PHTFTrace::getInstance(dirStr)->getFs();
    FileSystemMap fsm;
    for(int i = 0; i < fs->fileNum(); i ++)
    {
        fsm[fs->fileName(i)] = fs->fileSize(i);
    }
    FileBuilder::instance().populateFileSystem(fsm);
    cerr << "Done." << endl;
}

void PHTFIOApplication::performOpenProcessing(PHTFEventRecord* openRecord,
                                              int& outCommunicatorId)
{
    // Extract the descriptor number from the event record

    int fileId = openRecord->
        paramAsDescriptor(4, *phtfEvent_);

    // Extract the file name from the event record
    string fpt = openRecord->paramAt(1);
    Filename fn(phtfEvent_->memValue("String", fpt));

    // Construct a file descriptor for use in simulaiton
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(fn);
    assert(0 != fd);

    // Associate the file id with a file descriptor
    setDescriptor(fileId, fd);

    // Extract the communicator id
    string gstr = openRecord->paramAt(0);
    gstr = getAlias(gstr);
    outCommunicatorId = (int)strtol(gstr.c_str(), NULL, 0);
}

void PHTFIOApplication::performSeekProcessing(PHTFEventRecord* seekRecord)
{
    string dirStr = par("dirPHTF").stringValue();

    string hstr = seekRecord->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);

    string ostr = seekRecord->paramAt(1);
    long offset = strtol(ostr.c_str(), NULL, 16);

    string wstr = seekRecord->paramAt(3);
    int whence = (int)strtol(wstr.c_str(), NULL, 0);

    FileDescriptor* fd = getDescriptor(handle);

    if(whence == (int)strtol(PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_SEEK_SET").c_str(), 0, 10))
    {
        fd->setFilePointer(offset);
    }
    else if(whence == (int)strtol(PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_SEEK_CUR").c_str(), 0, 10))
    {
        fd->moveFilePointer(offset);
    }
    else if(whence == (int)strtol(PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_SEEK_END").c_str(), 0, 10))
    {
        fd->setFilePointer(fd->getMetaData()->size + offset);
    }
}

void PHTFIOApplication::performWaitProcessing(PHTFEventRecord* waitRecord,
                                              bool& outWaitIsComplete)
{
    // Extract the request id
    string str = waitRecord->paramAt(0);
    long requestId = strtol(str.c_str(), NULL, 16);

    // Determine if the operation being waiting on is pending
    outWaitIsComplete = false;
    if (0 == pendingRequestsById_.count(requestId))
    {
        outWaitIsComplete = true;
    }
}

void PHTFIOApplication::performSetViewProcessing(PHTFEventRecord* setViewRecord)
{
    string hstr = setViewRecord->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);
    FileDescriptor* fd = getDescriptor(handle);

    string ofstr = setViewRecord->paramAt(1);
    long offset = strtol(ofstr.c_str(), NULL, 0);

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
    string newcomm;
    switch(op)
    {
        case COMM_DUP:
            newcomm = commRecord->paramAt(1);
            break;
        case COMM_CREATE:
            break;
        case COMM_SPLIT:
            newcomm = commRecord->paramAt(3);
            break;
        case COMM_RANK:
            newcomm = commRecord->paramAt(0);
            break;
        default:
            cerr << "Error: unsupported comm op type: " << op << endl;
            assert(false);
            break;
    }
    performCreateCommunicator(newcomm);
}

void PHTFIOApplication::performCreateCommunicator(string newcomm)
{
    cout << "Create comm " << newcomm;
    string aliasComm = getAlias(newcomm);
    Communicator comm = (Communicator)strtol(aliasComm.c_str(), NULL, 0);

    cout << " alias " << hex << comm;

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

            cout << " add " << ss.str();
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
    cout << ": " << CommMan::instance().commSize(comm) << endl;
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

    cout << "Create datatype " << realtype << " ";

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
                return NULL;
            else
                return dataTypeById_[typeId];
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
    string dirStr = par("dirPHTF").stringValue();
    string ssize = PHTFTrace::getInstance(dirStr)->getFs()->consts(typeId);
    if(!ssize.compare(""))
    {
        ssize = phtfEvent_->memValue(typeId, "size");
        if(!ssize.compare(""))
        {
            return;
        }
    }

    size = (size_t)strtol(ssize.c_str(), 0, 10);

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
            cerr << "Error: unsupported basic type: width " << size << endl;
            assert(false);
            break;
    }

    dataTypeById_[typeId] = newDataType;
    cout << "Data Type #: " << dataTypeById_.size() << endl;
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
    cout << "Data Type #: " << dataTypeById_.size() << endl;
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
    vector<DataType*> oldTypes;

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
    cout << "Data Type #: " << dataTypeById_.size() << endl;
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
    cout << "Data Type #: " << dataTypeById_.size() << endl;
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
    cout << "Data Type #: " << dataTypeById_.size() << endl;
}

spfsMPIBarrierRequest* PHTFIOApplication::createBarrierMessage(
    const PHTFEventRecord* barrierRecord)
{
    string str = barrierRecord->paramAt(0);
    str = getAlias(str);

    int comm = (int)strtol(str.c_str(), NULL, 0);
    assert(-1 != comm);

    spfsMPIBarrierRequest* barrier =
        new spfsMPIBarrierRequest(0, SPFS_MPI_BARRIER_REQUEST);
    barrier->setCommunicator(comm);
    return barrier;
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

cMessage* PHTFIOApplication::createCPUPhaseMessage(
    const PHTFEventRecord* cpuRecord)
{
    double delay = cpuRecord->duration();

    cMessage *msg = new cMessage("CPU Phase");
    cPar *cp = new cPar("Delay");
    *cp = delay;
    msg->addPar(cp);
    return msg;
}

spfsMPIFileCloseRequest* PHTFIOApplication::createCloseMessage(
    const PHTFEventRecord* closeRecord)
{
    spfsMPIFileCloseRequest* close = new spfsMPIFileCloseRequest(
        0, SPFS_MPI_FILE_CLOSE_REQUEST);
    return close;
}

spfsMPIFileDeleteRequest* PHTFIOApplication::createDeleteRequest(
    const PHTFEventRecord* deleteRecord)
{
    // Extract the file name
    string filename = deleteRecord->paramAsFilename(0, *phtfEvent_);

    // Fill out the delete request
    spfsMPIFileDeleteRequest* deleteRequest =
        new spfsMPIFileDeleteRequest(0, SPFS_MPI_FILE_DELETE_REQUEST);
    deleteRequest->setFileName(filename.c_str());
    cerr << __FILE__ << ":" << __LINE__ << ": Deleting " << filename << endl;
    return deleteRequest;
}

spfsMPIFileOpenRequest* PHTFIOApplication::createOpenMessage(
    const PHTFEventRecord* openRecord)
{
    // Extract the descriptor id

    int fileId = openRecord->
        paramAsDescriptor(4, *phtfEvent_);

    // Retrieve the descriptor
    FileDescriptor* fd = getDescriptor(fileId);
    assert(0 != fd);

    // Extract the open mode
    int mode = (int)strtol(
        openRecord->paramAt(2).c_str(), NULL, 0);

    // Extract the communicator id
    string gstr = openRecord->paramAt(0);
    gstr = getAlias(gstr);
    int communicatorId = (int)strtol(gstr.c_str(), NULL, 0);
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Opening file with comm: " << communicatorId << endl;

    // Fill out the open request
    spfsMPIFileOpenRequest* open = new spfsMPIFileOpenRequest(
        0, SPFS_MPI_FILE_OPEN_REQUEST);
    open->setCommunicator(communicatorId);
    open->setFileName(fd->getFilename().c_str());
    open->setFileDes(fd);
    open->setMode(mode);
    return open;
}


spfsMPIFileReadAtRequest* PHTFIOApplication::createReadAtMessage(
    const PHTFEventRecord* readAtRecord)
{
    string ofstr = readAtRecord->paramAt(1);
    long offset = strtol(ofstr.c_str(), NULL, 0);

    string ctstr = readAtRecord->paramAt(3);
    long count = strtol(ctstr.c_str(), NULL, 16);

    string hstr = readAtRecord->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);
    FileDescriptor* fd = getDescriptor(handle);

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

spfsMPIFileReadAtRequest* PHTFIOApplication::createReadMessage(
    const PHTFEventRecord* readRecord)
{
    string ctstr = readRecord->paramAt(2);
    long count = strtol(ctstr.c_str(), NULL, 16);

    string hstr = readRecord->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);
    FileDescriptor* fd = getDescriptor(handle);

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

    fd->moveFilePointer(count);

    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createIReadMessage(
    const PHTFEventRecord* readRecord)
{
    string str = readRecord->paramAt(4);
    long reqid = strtol(str.c_str(), NULL, 16);

    spfsMPIFileReadAtRequest* iread = createReadMessage(readRecord);
    iread->setReqId(reqid);

    // Update the list of non-blocking operations still pending
    pendingRequestsById_[reqid] = iread;
    return iread;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createWriteAtMessage(
    const PHTFEventRecord* writeAtRecord)
{
    string ofstr = writeAtRecord->paramAt(1);
    long offset = strtol(ofstr.c_str(), NULL, 0);

    string ctstr = writeAtRecord->paramAt(3);
    long count = strtol(ctstr.c_str(), NULL, 16);

    string hstr = writeAtRecord->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);
    FileDescriptor* fd = getDescriptor(handle);

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

spfsMPIFileWriteAtRequest* PHTFIOApplication::createWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    string ctstr = writeRecord->paramAt(2);
    long count = strtol(ctstr.c_str(), NULL, 16);

    string hstr = writeRecord->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);
    FileDescriptor* fd = getDescriptor(handle);

    string dtstr = writeRecord->paramAt(3);
    dtstr = getAlias(dtstr);
    DataType* dataType = getDataTypeById(dtstr);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setDataType(dataType->clone());
    write->setCount(count);
    write->setOffset(fd->getFilePointer());
    write->setReqId(-1);
    write->setFileDes(fd);

    fd->moveFilePointer(count);

    return write;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createIWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    string str = writeRecord->paramAt(4);
    long reqid = strtol(str.c_str(), NULL, 16);

    spfsMPIFileWriteAtRequest* iwrite = createWriteMessage(writeRecord);
    iwrite->setReqId(reqid);

    // Update the list of non-blocking operations still pending
    pendingRequestsById_[reqid] = iwrite;
    return iwrite;
}

string PHTFIOApplication::getAlias(string id)
{
    string alias = phtfEvent_->memValue("Alias", id);
    if(alias.compare(""))
        return getAlias(alias);
    else
    {
        stringstream ss("");
        ss << "0x" << id.substr(id.length() - 8);
        alias = phtfEvent_->memValue("Alias", ss.str());
        if(alias.compare(""))
            return getAlias(alias);
        else
            return id;
    }
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
