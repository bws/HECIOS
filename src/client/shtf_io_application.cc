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
#include "shtf_io_application.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
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
Define_Module(SHTFIOApplication);


SHTFIOApplication::SHTFIOApplication()
    : IOApplication(),
      trace_(0),
      byteDataType_(BasicDataType::MPI_BYTE_WIDTH)
{
}

/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void SHTFIOApplication::initialize()
{
    IOApplication::initialize();

    // Get the trace file name and perform the rank substitution
    string traceName = par("traceFile").stringValue();
    trace_ = createIOTrace(traceName);
    assert(0 != trace_);
    
    // Send the kick start message
    cMessage* kickStart = new cMessage();
    scheduleAt(1.0, kickStart);
}

/**
 * Cleanup trace and tally statistics
 */
void SHTFIOApplication::finish()
{
    IOApplication::finish();

    // Delete open trace
    delete trace_;
    trace_ = 0;

}

bool SHTFIOApplication::scheduleNextMessage()
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

cMessage* SHTFIOApplication::createMessage(IOTrace::Record* rec)
{
    cMessage* mpiMsg = 0;

    // Create the correct messages for each operation type
    switch(rec->opType()) {
        case IOTrace::ACCESS:
        {
            mpiMsg = createGetAModeMessage(rec);
            break;
        }
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
        case IOTrace::STAT:
        {
            mpiMsg = createGetSizeMessage(rec);
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

void SHTFIOApplication::populateFileSystem()
{
    assert(0 != trace_);
    const FileSystemMap* traceFS = trace_->getFiles();
    FileBuilder::instance().populateFileSystem(*traceFS);
}

spfsMPIDirectoryCreateRequest* SHTFIOApplication::createDirectoryCreateMessage(
    const IOTrace::Record* mkdirRecord)
{
    assert(IOTrace::MKDIR == mkdirRecord->opType());
    spfsMPIDirectoryCreateRequest* createDir =
        new spfsMPIDirectoryCreateRequest(0, SPFS_MPI_DIRECTORY_CREATE_REQUEST);
    createDir->setDirName(mkdirRecord->filename().c_str());
    return createDir;
}

spfsMPIFileCloseRequest* SHTFIOApplication::createCloseMessage(
    const IOTrace::Record* closeRecord)
{
    assert(IOTrace::CLOSE == closeRecord->opType());

    // Remove and clean up the file descriptor
    FileDescriptor* fd = removeDescriptor(closeRecord->fileId());
    delete fd;
    
    spfsMPIFileCloseRequest* close = new spfsMPIFileCloseRequest(
        0, SPFS_MPI_FILE_CLOSE_REQUEST);
    return close;
}

spfsMPIFileGetAModeRequest* SHTFIOApplication::createGetAModeMessage(
    const IOTrace::Record* accessRecord)
{
    assert(IOTrace::ACCESS == accessRecord->opType());

    spfsMPIFileGetAModeRequest* getAMode = new spfsMPIFileGetAModeRequest(
        0, SPFS_MPI_FILE_GET_AMODE_REQUEST);
    getAMode->setFileName(accessRecord->filename().c_str());
    return getAMode;
}

spfsMPIFileGetSizeRequest* SHTFIOApplication::createGetSizeMessage(
    const IOTrace::Record* statRecord)
{
    assert(IOTrace::STAT == statRecord->opType());

    spfsMPIFileGetSizeRequest* getSize = new spfsMPIFileGetSizeRequest(
        0, SPFS_MPI_FILE_GET_SIZE_REQUEST);
    getSize->setFileName(statRecord->filename().c_str());
    return getSize;
}

spfsMPIFileOpenRequest* SHTFIOApplication::createOpenMessage(
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

spfsMPIFileReadAtRequest* SHTFIOApplication::createReadMessage(
    const IOTrace::Record* readRecord)
{
    assert(IOTrace::READ == readRecord->opType());

    FileDescriptor* fd = getDescriptor(readRecord->fileId());

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(readRecord->length());
    read->setDataType(&byteDataType_);
    read->setOffset(readRecord->offset());
    read->setFileDes(fd);
    return read;
}


spfsMPIFileReadAtRequest* SHTFIOApplication::createReadAtMessage(
    const IOTrace::Record* readAtRecord)
{
    assert(IOTrace::READ_AT == readAtRecord->opType());

    FileDescriptor* fd = getDescriptor(readAtRecord->fileId());

    spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
        0, SPFS_MPI_FILE_READ_AT_REQUEST);
    read->setCount(readAtRecord->length());
    read->setDataType(&byteDataType_);
    read->setOffset(readAtRecord->offset());
    read->setFileDes(fd);
    return read;
}

spfsMPIFileUpdateTimeRequest* SHTFIOApplication::createUpdateTimeMessage(
    const IOTrace::Record* utimeRecord)
{
    assert(IOTrace::UTIME == utimeRecord->opType());

    spfsMPIFileUpdateTimeRequest* utime = new spfsMPIFileUpdateTimeRequest(
        0, SPFS_MPI_FILE_UPDATE_TIME_REQUEST);
    utime->setFileName(utimeRecord->filename().c_str());
    return utime;
}


spfsMPIFileWriteAtRequest* SHTFIOApplication::createWriteAtMessage(
    const IOTrace::Record* writeAtRecord)
{
    assert(IOTrace::WRITE_AT == writeAtRecord->opType());

    FileDescriptor* fd = getDescriptor(writeAtRecord->fileId());

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setCount(writeAtRecord->length());
    write->setDataType(&byteDataType_);
    write->setOffset(writeAtRecord->offset());
    write->setFileDes(fd);
    return write;
}

spfsMPIFileWriteAtRequest* SHTFIOApplication::createWriteMessage(
    const IOTrace::Record* writeRecord)
{
    assert(IOTrace::WRITE == writeRecord->opType());

    FileDescriptor* fd = getDescriptor(writeRecord->fileId());

    spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
        0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
    write->setCount(writeRecord->length());
    write->setDataType(&byteDataType_);
    write->setOffset(writeRecord->offset());
    write->setFileDes(fd);
    return write;
}

IOTrace* SHTFIOApplication::createIOTrace(const string& traceFilename)
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

UMDIOTrace* SHTFIOApplication::createUMDIOTrace(string traceFilename)
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

SHTFIOTrace* SHTFIOApplication::createSHTFIOTrace(const string& traceFilename)
{
    SHTFIOTrace* trace = 0;
    try
    {
        trace = new SHTFIOTrace(traceFilename);
    } catch(...)
    {
    }
    return trace;
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
