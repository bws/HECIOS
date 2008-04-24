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
#include "mpi_mid_m.h"
#include "storage_layout_manager.h"
#include "phtf_io_trace.h"
#include "comm_man.h"

using namespace std;

// OMNet Registriation Method
Define_Module(PHTFIOApplication);


/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void PHTFIOApplication::initialize()
{
    string dirStr = par("dirPHTF").stringValue();

    // set value for MPI_COMM_WORLD & MPI_COMM_SELF based on configuration in fs.ini
    CommMan::getInstance()->commWorld(
        (int)strtol(PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_COMM_WORLD").c_str(),
                    0, 10));
    
    CommMan::getInstance()->commSelf(
        (int)strtol(PHTFTrace::getInstance(dirStr)->getFs()->consts("MPI_COMM_SELF").c_str(),
                    0, 10));

    // parent class initilization, set rank_ and stuff
    IOApplication::initialize();

    PHTFEventRecord::buildOpMap();

    // init some variable
    context_ = 0;
    barrierCounter_ = 0;
    blocked_ = false;
    noGetNext_ = false;
    
    // Send the kick start message
    cMessage* kickStart = new cMessage();
    scheduleAt(1.0, kickStart);
}

/**
 * Cleanup trace and tally statistics
 */
void PHTFIOApplication::finish()
{
    IOApplication::finish();

    phtfEvent_->close();
}

/** @param acitve - whether bcast barrier msg */
void PHTFIOApplication::handleBarrier(cMessage* msg, bool active)
{
    if(active)
    {
        spfsMPIBcastRequest *req =
            new spfsMPIBcastRequest(0, SPFS_MPI_BCAST_REQUEST);
                
        req->setRoot(
            CommMan::getInstance()->commRank(
                dynamic_cast<spfsMPIMidBarrierRequest*>(msg)->getCommunicator(),
                rank_));

        req->setCommunicator(
            dynamic_cast<spfsMPIMidBarrierRequest*>(msg)->getCommunicator());
        req->encapsulate(msg);
        send(req, mpiOutGate_);
    }

    barrierCounter_ --;

    cerr << rank_ << " barrier: " << barrierCounter_ << endl;

    if(barrierCounter_ == 0 || (active && barrierCounter_ < 0))
    {
        cerr << rank_ << " break barrier!" << endl;
        scheduleAt(simTime(), new cMessage());
    }
}

bool PHTFIOApplication::scheduleNextMessage()
{
    if(!phtfEvent_)
    {
        string dirStr = par("dirPHTF").stringValue();
        // get event file for this ioapp
        phtfEvent_ = PHTFTrace::getInstance(dirStr)->getEvent(rank_);
        phtfEvent_->open();
    }
    
    cMessage * msg = 0;

    bool msgScheduled = false;

    if(!blocked_)
    {
        while(!phtfEvent_->eof())
        {
            if(!noGetNext_)*phtfEvent_ >> phtfRecord_;
            msg = createMessage(&phtfRecord_);
            if(!msg) continue;
            cerr << "Rank " << rank_ << " IOApplication Time: " << simTime() << ": " << phtfRecord_.recordStr() << endl;
            switch(phtfRecord_.recordOp())
            {
                case CPU_PHASE:
                    scheduleCPUMessage(msg);
                    break;
                case BARRIER:
                    handleBarrier(msg, true);
                    break;
                case WAIT:
                    delete msg;
                    break;
                case SEEK:
                    scheduleAt(simTime(), msg);
                    break;
                case OPEN:
                    if(CommMan::getInstance()->commSize(mostRecentGroup_) != 0)
                    {
                        if(!noGetNext_)
                        {
                            noGetNext_ = true;
                            delete msg;
                            // barrier
                            PHTFEventRecord re;
                            stringstream ss("");
                            ss << mostRecentGroup_;
                            re.params(ss.str());
                            handleBarrier(createBarrierMessage(&re), true);
                            break;
                        }
                    }

                    if(CommMan::getInstance()->commRank(mostRecentGroup_, rank_) == 0)
                    {
                        send(msg, ioOutGate_);
                    }
                    else
                    {
                        context_ = msg;
                    }
                    noGetNext_ = false;
                    break;        
                default:
                    cerr << rank_ << " send msg " << msg->kind() << endl;
                    send(msg, ioOutGate_);
                    msgScheduled = true;
                    break;
            }
            break;
        }
    }
    return msgScheduled;
}

void PHTFIOApplication::handleMPIMessage(cMessage* msg)
{
    if(msg->kind() == SPFS_MPI_FILE_OPEN_RESPONSE)
    {
        msg->setContextPointer(context_);
        IOApplication::handleIOMessage(msg);
    }
    else if(msg->kind() == SPFS_MPIMID_BARRIER_REQUEST)
    {
        handleBarrier(msg);
        delete msg;
    }
    else
    {
        delete msg;
    }
}

void PHTFIOApplication::handleIOMessage(cMessage* msg)
{
    cMessage * orgMsg = (cMessage*)msg->contextPointer();

    if(orgMsg != 0)
    {
        long req = -1;
        if(orgMsg->kind() == SPFS_MPI_FILE_WRITE_AT_REQUEST)
        {
            req = dynamic_cast<spfsMPIFileWriteAtRequest*>(orgMsg)->getReqId();
        }
        else if(orgMsg->kind() == SPFS_MPI_FILE_READ_AT_REQUEST)
        {
            req = dynamic_cast<spfsMPIFileReadAtRequest*>(orgMsg)->getReqId();
        }

        if(req != -1)
        {
            if(nonBlockingReq_.count(req))
            {
                nonBlockingReq_.erase(req);
            }
            if(waitReqId_ == req)
            {
                waitReqId_ = -1;
                blocked_ = false;
            }
        }        
    }

    if(orgMsg != 0 && orgMsg->kind() == SPFS_MPI_FILE_OPEN_REQUEST)
    {
        // bcast open response
        if(CommMan::getInstance()->commSize(mostRecentGroup_) > 1)
        {
            spfsMPIBcastRequest *req =
                new spfsMPIBcastRequest(0, SPFS_MPI_BCAST_REQUEST);

            req->setRoot(CommMan::getInstance()->commRank(mostRecentGroup_, rank_));
            req->setCommunicator(mostRecentGroup_);
            req->encapsulate((cMessage*)msg->dup());
            send(req, mpiOutGate_);
        }
    }

    IOApplication::handleIOMessage(msg);
}

void PHTFIOApplication::scheduleCPUMessage(cMessage *msg)
{
    double schTime = simTime() + msg->par("Delay").doubleValue();
    scheduleAt(schTime , msg);
}

cMessage* PHTFIOApplication::createMessage(PHTFEventRecord* rec)
{
    cMessage* mpiMsg = 0;

    switch(rec->recordOp())
    {
        case BARRIER:
            mpiMsg = createBarrierMessage(rec);
            break;
        case CPU_PHASE:
            mpiMsg = createCPUPhaseMessage(rec);
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
        case WAIT:
            mpiMsg = createWaitMessage(rec);
            break;
        case SEEK:
            mpiMsg = createSeekMessage(rec);
            break;
        default:
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

cMessage* PHTFIOApplication::createBarrierMessage(
    const PHTFEventRecord* barrierRecord)
{
    string str = const_cast<PHTFEventRecord*>(barrierRecord)->paramAt(0);
    int comm = (int)strtol(str.c_str(), NULL, 10);
    barrierCounter_ += CommMan::getInstance()->commSize(comm);
    spfsMPIMidBarrierRequest *mpimsg = new spfsMPIMidBarrierRequest(0, SPFS_MPIMID_BARRIER_REQUEST);
    mpimsg->setCommunicator(comm);
    return mpimsg;
}

cMessage* PHTFIOApplication::createWaitMessage(
    const PHTFEventRecord* waitRecord)
{
    string str = const_cast<PHTFEventRecord*>(waitRecord)->paramAt(0);
    long reqid = strtol(str.c_str(), NULL, 16);
    waitReqId_ = reqid;
    if(nonBlockingReq_.count(reqid))
    {
        blocked_ = true;
        return new cMessage("wait");
    }
    else
    {
        return NULL;
    }
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
    spfsMPIFileOpenRequest* open = new spfsMPIFileOpenRequest(
        0, SPFS_MPI_FILE_OPEN_REQUEST);

    stringstream ss("");
    string hpt = const_cast<PHTFEventRecord*>(openRecord)->paramAt(4);

    ss << hpt << "@" << const_cast<PHTFEventRecord*>(openRecord)->recordId();

    string hstr = phtfEvent_->memValue("Pointer", ss.str());
    
    long handle = strtol(hstr.c_str(), NULL, 16);
    
    string fpt = const_cast<PHTFEventRecord*>(openRecord)->paramAt(1);
    Filename fn(phtfEvent_->memValue("String", fpt));

    // Construct a file descriptor for use in simulaiton
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(fn);
    
    assert(0 != fd);

    // Associate the file id with a file descriptor
    setDescriptor(handle, fd);

    string gstr = const_cast<PHTFEventRecord*>(openRecord)->paramAt(0);
    int group = (int)strtol(gstr.c_str(), NULL, 10);
    mostRecentGroup_ = group;
    
    if(CommMan::getInstance()->commRank(group, rank_) == 0)
    {
        int mode = (int)strtol(const_cast<PHTFEventRecord*>(openRecord)->paramAt(2).c_str(), NULL, 10);
        
        open->setFileName(fn.str().c_str());
        open->setFileDes(fd);
        open->setMode(mode);
    }
    
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

    nonBlockingReq_[reqid] = iread;
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

    nonBlockingReq_[reqid] = iwrite;
    return iwrite;
}

cMessage * PHTFIOApplication::createSeekMessage(
    const PHTFEventRecord* seekRecord)
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

    return new cMessage("seek");
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
