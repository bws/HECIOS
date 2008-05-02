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
        CommMan::instance().setCommWorld(strtol(commWorldValue.c_str(), 0, 10));

        string commSelfValue =
            PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_COMM_SELF");
        CommMan::instance().setCommSelf(strtol(commSelfValue.c_str(), 0, 10));

        // I have no idea what this does???
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

        int opcode = eventRecord.recordOp();
        if (CPU_PHASE == opcode)
        {
            cMessage* msg = createCPUPhaseMessage(&eventRecord);
            scheduleCPUMessage(msg);
            msgScheduled = true;
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
        case READ_AT:
            mpiMsg = createReadAtMessage(rec);
            break;
        case WRITE_AT:
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
    stringstream ss("");
    string hpt = const_cast<PHTFEventRecord*>(openRecord)->paramAt(4);
    ss << hpt << "@" << const_cast<PHTFEventRecord*>(openRecord)->recordId();
    string hstr = phtfEvent_->memValue("Pointer", ss.str());
    int fileId = strtol(hstr.c_str(), NULL, 16);

    // Extract the file name from the event record
    string fpt = const_cast<PHTFEventRecord*>(openRecord)->paramAt(1);
    Filename fn(phtfEvent_->memValue("String", fpt));

    // Construct a file descriptor for use in simulaiton
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(fn);    
    assert(0 != fd);

    // Associate the file id with a file descriptor
    setDescriptor(fileId, fd);

    // Extract the communicator id
    string gstr = const_cast<PHTFEventRecord*>(openRecord)->paramAt(0);
    outCommunicatorId = (int)strtol(gstr.c_str(), NULL, 10);    
}

void PHTFIOApplication::performSeekProcessing(PHTFEventRecord* seekRecord)
{
    string dirStr = par("dirPHTF").stringValue();
    
    string hstr = const_cast<PHTFEventRecord*>(seekRecord)->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);

    string ostr = const_cast<PHTFEventRecord*>(seekRecord)->paramAt(1);
    long offset = strtol(ostr.c_str(), NULL, 16);

    string wstr = const_cast<PHTFEventRecord*>(seekRecord)->paramAt(3);
    int whence = (int)strtol(wstr.c_str(), NULL, 10);

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

spfsMPIBarrierRequest* PHTFIOApplication::createBarrierMessage(
    const PHTFEventRecord* barrierRecord)
{
    string str = const_cast<PHTFEventRecord*>(barrierRecord)->paramAt(0);
    int comm = (int)strtol(str.c_str(), NULL, 10);
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
    double delay = const_cast<PHTFEventRecord*>(cpuRecord)->duration();
    
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

spfsMPIFileOpenRequest* PHTFIOApplication::createOpenMessage(
    const PHTFEventRecord* openRecord)
{
    // Extract the descriptor id
    stringstream ss("");
    string hpt = const_cast<PHTFEventRecord*>(openRecord)->paramAt(4);
    ss << hpt << "@" << const_cast<PHTFEventRecord*>(openRecord)->recordId();
    string hstr = phtfEvent_->memValue("Pointer", ss.str());
    int fileId = strtol(hstr.c_str(), NULL, 16);

    // Retrieve the descriptor
    FileDescriptor* fd = getDescriptor(fileId);
    assert(0 != fd);

    // Extract the open mode
    int mode = (int)strtol(
        const_cast<PHTFEventRecord*>(openRecord)->paramAt(2).c_str(), NULL, 10);

    // Extract the communicator id
    string gstr = const_cast<PHTFEventRecord*>(openRecord)->paramAt(0);
    int communicatorId = (int)strtol(gstr.c_str(), NULL, 10);    
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
    long length = 0, offset = 0;

    FileDescriptor* fd = getDescriptor(100);
    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(length);
    read->setDataType(dataType);
    read->setOffset(offset);
    read->setFileDes(fd);
    read->setReqId(-1);
    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createReadMessage(
    const PHTFEventRecord* readRecord)
{
    string ctstr = const_cast<PHTFEventRecord*>(readRecord)->paramAt(2);
    long count = strtol(ctstr.c_str(), NULL, 16);

    string hstr = const_cast<PHTFEventRecord*>(readRecord)->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);

    FileDescriptor* fd = getDescriptor(handle);
    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(count);
    read->setDataType(dataType);
    read->setFileDes(fd);
    read->setOffset(fd->getFilePointer());
    read->setReqId(-1);

    fd->moveFilePointer(count);
    
    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createIReadMessage(
    const PHTFEventRecord* readRecord)
{
    string str = const_cast<PHTFEventRecord*>(readRecord)->paramAt(4);
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
    long length = 10, offset = 0;

    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);
    FileDescriptor* fd = getDescriptor(100);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setCount(length);
    write->setDataType(dataType);
    write->setOffset(offset);
    write->setReqId(-1);
    write->setFileDes(fd);

    return write;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    string ctstr = const_cast<PHTFEventRecord*>(writeRecord)->paramAt(2);
    long count = strtol(ctstr.c_str(), NULL, 16);

    string hstr = const_cast<PHTFEventRecord*>(writeRecord)->paramAt(0);
    long handle = strtol(hstr.c_str(), NULL, 16);

    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);
    FileDescriptor* fd = getDescriptor(handle);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setDataType(dataType);
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
    string str = const_cast<PHTFEventRecord*>(writeRecord)->paramAt(4);
    long reqid = strtol(str.c_str(), NULL, 16);
    
    spfsMPIFileWriteAtRequest* iwrite = createWriteMessage(writeRecord);
    iwrite->setReqId(reqid);

    // Update the list of non-blocking operations still pending
    pendingRequestsById_[reqid] = iwrite;
    return iwrite;
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
