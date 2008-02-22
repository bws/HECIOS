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
#include "io_application.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include "basic_data_type.h"
#include "cache_proto_m.h"
#include "filename.h"
#include "file_builder.h"
#include "file_descriptor.h"
#include "io_trace.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
#include "shtf_io_trace.h"
#include "storage_layout_manager.h"
#include "umd_io_trace.h"

using namespace std;

// OMNet Registriation Method
Define_Module(IOApplication);
static int rank_seed = 0;

/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void IOApplication::initialize()
{
    // Cache the gate descriptors
    ioInGate_ = findGate("ioIn");
    ioOutGate_ = findGate("ioOut");
    mpiOutGate_ = findGate("mpiOut");
    mpiInGate_ = findGate("mpiIn");
    
    // Set the process rank
    rank_ = rank_seed++;

    // Get the trace file name and perform the rank substitution
    string traceName = par("traceFile").stringValue();
    trace_ = createIOTrace(traceName);
    assert(0 != trace_);
    
    // Send the kick start message
    cMessage* kickStart = new cMessage("Kickstart");
    scheduleAt(1.0, kickStart);
}

/**
 * Cleanup trace and tally statistics
 */
void IOApplication::finish()
{
    // Delete open descriptors
    map<int, FileDescriptor*>::iterator iter;
    for (iter = descriptorById_.begin(); iter != descriptorById_.end(); ++iter)
    {
        delete iter->second;
    }

    // Delete open trace
    delete trace_;
    trace_ = 0;

    // Reset the rank generator to 0
    rank_seed = 0;
}

/**
 * Handle MPI-IO Response messages
 */
void IOApplication::handleMessage(cMessage* msg)
{
    bool msgScheduled = false;
    if (msg->isSelfMessage())
    {
        // Create file system files only once
        static bool fileSystemPopulated = false;
        if (!fileSystemPopulated)
        {
            populateFileSystem();
            fileSystemPopulated = true;
        }
    
        // Schedule the next message
        msgScheduled = scheduleNextMessage();
        delete msg;
    }
    else if (msg->arrivalGateId() == ioInGate_)
    {
        switch(msg->kind())
        {
            case SPFS_MPI_DIRECTORY_CREATE_RESPONSE:
            case SPFS_MPI_FILE_CLOSE_RESPONSE:
            case SPFS_MPI_FILE_DELETE_RESPONSE:
            case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_GET_INFO_RESPONSE:
            case SPFS_MPI_FILE_OPEN_RESPONSE:
            case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
            case SPFS_MPI_FILE_SET_INFO_RESPONSE:
            case SPFS_MPI_FILE_SET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_READ_AT_RESPONSE:
            case SPFS_MPI_FILE_READ_RESPONSE:
            case SPFS_MPI_FILE_UPDATE_TIME_RESPONSE:
            case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
            case SPFS_MPI_FILE_WRITE_RESPONSE:
            {
                // Schedule the next message
                msgScheduled = scheduleNextMessage();
                break;
            }
            default:
                cerr << "IOApplication::handleMessage not yet implemented "
                     << "for kind: "<< msg->kind() << endl;
                break;
        }

        // Delete the originating request
        delete (cMessage*)msg->contextPointer();
        
        // Delete the response
        delete msg;
    }
    else if (msg->arrivalGateId() == mpiInGate_)
    {
        // TODO: forwarding to cache?
        assert(false);
        send(msg, ioOutGate_);
    }

    if (!msgScheduled)
    {
        assert(false == trace_->hasMoreRecords());
        cerr << "Rank " << rank_ << " IOApplication Time: " << simTime()
             << ": No more messages to post." << endl;
    }
}

bool IOApplication::scheduleNextMessage()
{
    bool msgScheduled = false;
    if (trace_->hasMoreRecords())
    {
        IOTrace::Record* traceRec = trace_->nextRecord();
        assert(0 != traceRec);

        if (IOTrace::CPU_PHASE == traceRec->opType())
        {
            double phaseLength = traceRec->duration();
            cMessage* cpuPhase = new cMessage("CPU Phase");
            scheduleAt(simTime() + phaseLength, cpuPhase);
            cerr << "Scheduling CPU Phase to last: " << phaseLength << endl;
        }
        else
        {
            cMessage* msg = createMessage(traceRec);
            assert(0 != msg);
            send(msg, ioOutGate_);
        }
        delete traceRec;
        msgScheduled = true;
    }
    return msgScheduled;
}

cMessage* IOApplication::getNextMessage()
{
    cMessage* msg = 0;
    do {
        IOTrace::Record* traceRec = trace_->nextRecord();
        if (traceRec)
        {
            msg = createMessage(traceRec);
            delete traceRec;
        }

    } while (0 == msg && trace_->hasMoreRecords());

    return msg;
}

cMessage* IOApplication::createMessage(IOTrace::Record* rec)
{
    cMessage* mpiMsg = 0;

    // Create the correct messages for each operation type
    switch(rec->opType()) {
        case IOTrace::CLOSE:
        {
            mpiMsg = createCloseMessage(rec);
            break;
        }
        case IOTrace::MKDIR:
        {
            mpiMsg = createDirectoryCreateMessage(rec);
            break;
        }
        case IOTrace::OPEN:
        {
            mpiMsg = createOpenMessage(rec);
            break;
        }
        case IOTrace::READ_AT:
        {
            mpiMsg = createReadAtMessage(rec);
            break;
        }
        case IOTrace::READ:
        {
            mpiMsg = createReadMessage(rec);
            break;
        }
        case IOTrace::UTIME:
        {
            mpiMsg = createUpdateTimeMessage(rec);
            break;
        }
        case IOTrace::WRITE_AT:
        {
            mpiMsg = createWriteAtMessage(rec);
            break;
        }
        case IOTrace::WRITE:
        {
            mpiMsg = createWriteMessage(rec);
            break;
        }
        case IOTrace::SEEK:
        {
            // Retrieve the open file descriptor and adjust the file pointer
            // according to the seek parameters
            FileDescriptor* fd = getDescriptor(rec->fileId());
            fd->setFilePointer(rec->offset() + rec->length());
            break;
        }
        default:
            cerr << "Ignored IO OpType for MPI Application: " << rec->opType()
                 << endl;
            break;
    }
    return mpiMsg;    
}

void IOApplication::populateFileSystem()
{
    assert(0 != trace_);
    const FileSystemMap* traceFS = trace_->getFiles();
    FileBuilder::instance().populateFileSystem(*traceFS);
}

spfsMPIDirectoryCreateRequest* IOApplication::createDirectoryCreateMessage(
    const IOTrace::Record* mkdirRecord)
{
    assert(IOTrace::MKDIR == mkdirRecord->opType());
    spfsMPIDirectoryCreateRequest* createDir =
        new spfsMPIDirectoryCreateRequest(0, SPFS_MPI_DIRECTORY_CREATE_REQUEST);
    createDir->setDirName(mkdirRecord->filename().c_str());
    return createDir;
}

spfsMPIFileCloseRequest* IOApplication::createCloseMessage(
    const IOTrace::Record* closeRecord)
{
    assert(IOTrace::CLOSE == closeRecord->opType());
    spfsMPIFileCloseRequest* close = new spfsMPIFileCloseRequest(
        0, SPFS_MPI_FILE_CLOSE_REQUEST);
    return close;
}

spfsMPIFileOpenRequest* IOApplication::createOpenMessage(
    const IOTrace::Record* openRecord)
{
    assert(IOTrace::OPEN == openRecord->opType());

    // Determine if this file exists in the simulator
    Filename openFile(openRecord->filename());
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(openFile);
    assert(!((0 != fd) xor openRecord->fileExists()));
    
    // Associate the file id with a file descriptor
    setDescriptor(openRecord->fileId(), fd);

    // Create the open request
    spfsMPIFileOpenRequest* open = new spfsMPIFileOpenRequest(
        0, SPFS_MPI_FILE_OPEN_REQUEST);
    open->setFileName(openFile.str().c_str());
    
    open->setFileDes(fd);

    // Create the open access mode
    int accessMode = 0;
    if (openRecord->isCreate())
        accessMode |= MPI_MODE_CREATE;
    
    if (openRecord->isReadOnly())
        accessMode |= MPI_MODE_RDONLY;
    
    if (openRecord->isWriteOnly())
        accessMode |= MPI_MODE_WRONLY;

    if (openRecord->isReadWrite())
        accessMode |= MPI_MODE_RDWR;

    if (openRecord->isDeleteOnClose())
        accessMode |= MPI_MODE_DELETE_ON_CLOSE;

    if (openRecord->isExclusive())
        accessMode |= MPI_MODE_EXCL;

    if (openRecord->isAppend())
        accessMode |= MPI_MODE_APPEND;

    open->setMode(accessMode);
    
    return open;
}

spfsMPIFileReadAtRequest* IOApplication::createReadAtMessage(
    const IOTrace::Record* readAtRecord)
{
    assert(IOTrace::READ_AT == readAtRecord->opType());

    FileDescriptor* fd = getDescriptor(readAtRecord->fileId());
    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(readAtRecord->length());
    read->setDataType(dataType);
    read->setOffset(readAtRecord->offset());
    read->setFileDes(fd);
    return read;
}

spfsMPIFileReadAtRequest* IOApplication::createReadMessage(
    const IOTrace::Record* readRecord)
{
    assert(IOTrace::READ == readRecord->opType());

    FileDescriptor* fd = getDescriptor(readRecord->fileId());
    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(readRecord->length());
    read->setDataType(dataType);
    read->setOffset(readRecord->offset());
    read->setFileDes(fd);
    return read;
}

spfsMPIFileUpdateTimeRequest* IOApplication::createUpdateTimeMessage(
    const IOTrace::Record* utimeRecord)
{
    assert(IOTrace::UTIME == utimeRecord->opType());

    spfsMPIFileUpdateTimeRequest* utime = new spfsMPIFileUpdateTimeRequest(
        0, SPFS_MPI_FILE_UPDATE_TIME_REQUEST);
    utime->setFileName(utimeRecord->filename().c_str());
    return utime;
}

spfsMPIFileWriteAtRequest* IOApplication::createWriteAtMessage(
    const IOTrace::Record* writeAtRecord)
{
    assert(IOTrace::WRITE_AT == writeAtRecord->opType());

    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);
    FileDescriptor* fd = getDescriptor(writeAtRecord->fileId());

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setCount(writeAtRecord->length());
    write->setDataType(dataType);
    write->setOffset(writeAtRecord->offset());
    write->setFileDes(fd);

    return write;
}

spfsMPIFileWriteAtRequest* IOApplication::createWriteMessage(
    const IOTrace::Record* writeRecord)
{
    assert(IOTrace::WRITE == writeRecord->opType());

    DataType* dataType = new BasicDataType(BasicDataType::MPI_BYTE_WIDTH);
    FileDescriptor* fd = getDescriptor(writeRecord->fileId());

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setCount(writeRecord->length());
    write->setDataType(dataType);
    write->setOffset(writeRecord->offset());
    write->setFileDes(fd);

    return write;
}

void IOApplication::setDescriptor(int fileId, FileDescriptor* descriptor)
{
    assert((0 == descriptor) && (-1 != fileId) ? false : true);
    assert((0 != descriptor) && (-1 == fileId) ? false : true);
    descriptorById_[fileId] = descriptor;
}

FileDescriptor* IOApplication::getDescriptor(int fileId) const
{
    FileDescriptor* descriptor = 0;
    map<int, FileDescriptor*>::const_iterator iter =
        descriptorById_.find(fileId);
    if (iter != descriptorById_.end())
    {
        descriptor = iter->second;
    }
    return descriptor;
}

void IOApplication::invalidateCaches(spfsMPIFileWriteAtRequest* writeAt)
{
    // send msg to mpiOut, encapsulating spfsCacheInvalidateRequest
    cMessage* inval = createCacheInvalidationMessage(writeAt);
    spfsMPIBcastRequest* req =
        new spfsMPIBcastRequest("MPI_BCAST", SPFS_MPI_BCAST_REQUEST);
    
    req->setRoot(this->rank_);
    req->setIsGlobal(true);
    req->encapsulate(inval);
    send(req, mpiOutGate_);
}

spfsCacheInvalidateRequest* IOApplication::createCacheInvalidationMessage(
    spfsMPIFileWriteAtRequest* writeAt)
{
    spfsCacheInvalidateRequest* invalidator = new spfsCacheInvalidateRequest();
    FSHandle handle = writeAt->getFileDes()->getMetaData()->handle;
    invalidator->setHandle(handle);
    invalidator->setOffset(writeAt->getOffset());
    invalidator->setDataType(writeAt->getDataType());
    invalidator->setCount(writeAt->getCount());

    return invalidator;
}

IOTrace* IOApplication::createIOTrace(const string& traceFilename)
{
    IOTrace* trace = 0;
    string::size_type pos = traceFilename.find_last_of('.');
    string extension = traceFilename.substr(pos + 1);
    if ("shtf" == extension)
    {
        trace = createSHTFIOTrace(traceFilename);
    }
    else if ("trace" == extension)
    {
        trace = createUMDIOTrace(traceFilename);
    }
    else
    {
        cerr << "Tracefile extension not recognized: " << extension << endl
             << "Valid extensions are: shtf or trace" << endl;;
    }
    return trace;
}

UMDIOTrace* IOApplication::createUMDIOTrace(string traceFilename)
{
    // Perform %r subsititution if neccesary
    long numTraceProcs = par("numTraceProcs").longValue();
    string::size_type replaceIdx = traceFilename.find("%r");
    if (string::npos != replaceIdx)
    {
        long fileRank = rank_ % numTraceProcs;
        stringstream rankStr;
        rankStr << fileRank;
        traceFilename.replace(replaceIdx, 2, rankStr.str());
    }

    return new UMDIOTrace(numTraceProcs, traceFilename);
}

SHTFIOTrace* IOApplication::createSHTFIOTrace(const string& traceFilename)
{
    return new SHTFIOTrace(traceFilename);
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
