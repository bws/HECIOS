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
#include "pfs_utils.h"
#include "storage_layout_manager.h"
#include "phtf_io_trace.h"

using namespace std;

// OMNet Registriation Method
Define_Module(PHTFIOApplication);


/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void PHTFIOApplication::initialize()
{
    IOApplication::initialize();

    string dirStr = par("dirPHTF").stringValue();
    phtfEvent_ = PHTFTrace::getInstance(dirStr)->getEvent(rank_);
    phtfEvent_->open();
    PHTFEventRecord::buildOpMap();
    
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

bool PHTFIOApplication::scheduleNextMessage()
{
    cMessage * msg = 0;
    PHTFEventRecord re;
    bool msgScheduled = false;
    while(!phtfEvent_->eof()){
        *phtfEvent_ >> re;
        msg = createMessage(&re);
        if(msg) {
            if(re.recordOp() == CPU_PHASE) {
                double schTime = simTime() + msg->par("Delay").doubleValue();
                cerr << simTime() << " : Rank " << rank_ << " cpu phase " << msg->par("Delay").doubleValue() << " at " << schTime << endl;
                scheduleAt(schTime , msg);
            }
            else {
                cerr << simTime() << " : Rank " << rank_ << " send msg kind " << msg->kind() << endl;
                send(msg, ioOutGate_);
            }

            msgScheduled = true;
            
            break;
        }
    }
    
    return msgScheduled;
}

cMessage* PHTFIOApplication::createMessage(PHTFEventRecord* rec)
{
    cMessage* mpiMsg = 0;

    switch(rec->recordOp())
        {
        case BARRIER:
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
        default:
            break;
        }
    return mpiMsg;
}

void PHTFIOApplication::populateFileSystem()
{
    cerr << "Populating file system . . . ";
    FileSystemMap fsm;
    fsm["/test"] = 10000;
    fsm["/12345"] = 100;
    FileBuilder::instance().populateFileSystem(fsm);
    cerr << "Done." << endl;
}

cMessage* PHTFIOApplication::createCPUPhaseMessage(
    const PHTFEventRecord* cpuRecord)
{
    double delay = const_cast<PHTFEventRecord*>(cpuRecord)->duration();
    
    cMessage *msg = new cMessage("CPU");
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
    // If the file does not exist, create it
    Filename openFile("/test");
    
    // Construct a file descriptor for use in simulaiton
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(openFile);

    // Associate the file id with a file descriptor
    setDescriptor(100, fd);
    
    spfsMPIFileOpenRequest* open = new spfsMPIFileOpenRequest(
        0, SPFS_MPI_FILE_OPEN_REQUEST);
    open->setFileName(openFile.str().c_str());
    open->setFileDes(fd);
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
    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createReadMessage(
    const PHTFEventRecord* readRecord)
{
    long count = 100;

    FileDescriptor* fd = getDescriptor(100);
    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_REQUEST);
    read->setCount(count);
    read->setDataType(dataType);
    read->setFileDes(fd);
    read->setOffset(0);
    return read;
}

spfsMPIFileReadAtRequest* PHTFIOApplication::createIReadMessage(
    const PHTFEventRecord* readRecord)
{
    spfsMPIFileReadAtRequest* iread = createReadMessage(readRecord);
    iread->setKind(SPFS_MPI_FILE_READ_AT_REQUEST);
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
    write->setFileDes(fd);

    // Generate corresponding cache invalidation messages
    //invalidateCaches(write);
    return write;
}


spfsMPIFileWriteAtRequest* PHTFIOApplication::createWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    string ctstr = const_cast<PHTFEventRecord*>(writeRecord)->paramAt(2);
    long count = atol(ctstr.c_str());

    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);
    FileDescriptor* fd = getDescriptor(100);

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_REQUEST);
    write->setDataType(dataType);
    write->setCount(count);
    write->setOffset(0);
    write->setFileDes(fd);

    return write;
}

spfsMPIFileWriteAtRequest* PHTFIOApplication::createIWriteMessage(
    const PHTFEventRecord* writeRecord)
{
    spfsMPIFileWriteAtRequest* iwrite = createWriteMessage(writeRecord);
    iwrite->setKind(SPFS_MPI_FILE_WRITE_AT_REQUEST);
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
