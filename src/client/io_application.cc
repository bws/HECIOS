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
#include "cache_proto_m.h"
#include "filename.h"
#include "file_builder.h"
#include "mpi_proto_m.h"
#include "pfs_utils.h"
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
    mpiClientOutGate_ = findGate("mpiClientOut");
    mpiServerInGate_ = findGate("mpiServerIn");
    
    // Set the process rank
    rank_ = rank_seed++;

    // Get the trace file name and perform the rank substitution
    string traceName = par("traceFile").stringValue();
    string::size_type replaceIdx = traceName.find("%r");
    if (string::npos != replaceIdx)
    {
        long numTraceProcs = par("numTraceProcs").longValue();
        long fileRank = rank_ % numTraceProcs;
        stringstream rankStr;
        rankStr << fileRank;
        traceName.replace(replaceIdx, 2, rankStr.str());
    }
    
    // Construct a trace and begin sending events
    long size = par("numTraceProcs").longValue();
    trace_ = new UMDIOTrace(size, traceName);

    // Send the kick start message
    cMessage* kickStart = new cMessage();
    scheduleAt(1.0, kickStart);
}

/**
 * Cleanup trace and tally statistics
 */
void IOApplication::finish()
{
    // Delete open descriptors
    map<int, FSOpenFile*>::iterator iter;
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
    if (msg->isSelfMessage())
    {
        // On a self message kick start, get the first message
        delete msg;
        cMessage* firstMessage = getNextMessage();
        send(firstMessage, ioOutGate_);
    }
    else if (msg->arrivalGateId() == ioInGate_)
    {
        switch(msg->kind())
        {
            case SPFS_MPI_FILE_OPEN_RESPONSE:
            case SPFS_MPI_FILE_CLOSE_RESPONSE:
            case SPFS_MPI_FILE_DELETE_RESPONSE:
            case SPFS_MPI_FILE_SET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
            case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_GET_INFO_RESPONSE:
            case SPFS_MPI_FILE_SET_INFO_RESPONSE:
            case SPFS_MPI_FILE_READ_AT_RESPONSE:
            case SPFS_MPI_FILE_READ_RESPONSE:
            case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
            case SPFS_MPI_FILE_WRITE_RESPONSE:
            {
                // Send the next message
                cMessage* nextMsg = getNextMessage();
                if (0 != nextMsg)
                {
                    //cerr << "\nNext application message posted: "
                    //     << nextMsg->kind() << endl;
                    send(nextMsg, ioOutGate_);
                }
                else
                {
                    cerr << "IOApplication: No more messages to post."
                         << endl;
                }
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
    else if (msg->arrivalGateId() == mpiServerInGate_)
    {
        cMessage* cacheMsg = msg->decapsulate();
        delete msg;
        send(cacheMsg, ioOutGate_);
    }
}

cMessage* IOApplication::getNextMessage()
{
    cMessage* msg = 0;
    do {
        IOTraceRecord* traceRec = trace_->nextRecord();
        if (traceRec)
        {
            msg = createMessage(traceRec);
            delete traceRec;
        }

    } while (0 == msg && trace_->hasMoreRecords());

    return msg;
}

cMessage* IOApplication::createMessage(IOTraceRecord* rec)
{
    cMessage* mpiMsg = 0;

    // Create the correct messages for each operation type
    switch(rec->opType()) {
        case IOTrace::OPEN:
        {
            // If the file does not exist, create it
            Filename openFile(trace_->getFilename(rec->fileId()));
            if (!FileBuilder::instance().fileExists(openFile))
            {
                // Create trace files as needed
                int numServers = FileBuilder::instance().getNumDataServers();
                FileBuilder::instance().createFile(openFile, 0, numServers);
            }
            
            spfsMPIFileOpenRequest* open = new spfsMPIFileOpenRequest(
                0, SPFS_MPI_FILE_OPEN_REQUEST);
            open->setFileName(openFile.str().c_str());

            // Construct a file descriptor for use in simulaiton
            FSOpenFile* descriptor = new FSOpenFile();
            setDescriptor(rec->fileId(), descriptor);
            open->setFileDes(descriptor);
            mpiMsg = open;
            break;
        }
        case IOTrace::CLOSE:
        {
            spfsMPIFileCloseRequest* close = new spfsMPIFileCloseRequest(
                0, SPFS_MPI_FILE_CLOSE_REQUEST);
            mpiMsg = close;
            break;
        }
        case IOTrace::READ_AT:
        {
            spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
                0, SPFS_MPI_FILE_READ_AT_REQUEST);
            read->setCount(1);
            read->setDataType(rec->length());
            read->setOffset(rec->offset());
            FSOpenFile* descriptor = getDescriptor(rec->fileId());
            read->setFileDes(descriptor);
            mpiMsg = read;
            break;
        }
        case IOTrace::WRITE_AT:
        {
            spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
                0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
            write->setCount(1);
            write->setDataType(rec->length());
            write->setOffset(rec->offset());
            FSOpenFile* descriptor = getDescriptor(rec->fileId());
            write->setFileDes(descriptor);
            mpiMsg = write;

            // Generate corresponding cache invalidation messages
            invalidateCaches(write);
            break;
        }
        case IOTrace::SEEK:
        {
            // Retrieve the open file descriptor and adjust the file pointer
            // according to the seek parameters
            FSOpenFile* descriptor = getDescriptor(rec->fileId());
            descriptor->filePtr = rec->offset() + rec->length();
            break;
        }
        default:
            cerr << "Ignored IO OpType for MPI Application: " << rec->opType()
                 << endl;
            break;
    }
    return mpiMsg;    
}

void IOApplication::setDescriptor(int fileId, FSOpenFile* descriptor)
{
    descriptorById_[fileId] = descriptor;
}

FSOpenFile* IOApplication::getDescriptor(int fileId) const
{
    FSOpenFile* descriptor = 0;
    map<int, FSOpenFile*>::const_iterator iter = descriptorById_.find(fileId);
    if (iter != descriptorById_.end())
    {
        descriptor = iter->second;
    }
    return descriptor;
}

void IOApplication::invalidateCaches(spfsMPIFileWriteAtRequest* writeAt)
{
    cMessage* inval = createCacheInvalidationMessage(writeAt);
    vector<IPvXAddress*> ips = PFSUtils::instance().getAllRankIP();

    for (unsigned int i = 0; i < ips.size(); i++)
    {
        if (i != rank_)
        {
            spfsMPISendRequest* msg = new spfsMPISendRequest();
            msg->setRank(i);
            msg->encapsulate(static_cast<cMessage*>(inval->dup()));
            send(msg, mpiClientOutGate_);
        }
    }

    // Cleanup memory
    delete inval;
}

spfsCacheInvalidateRequest* IOApplication::createCacheInvalidationMessage(
    spfsMPIFileWriteAtRequest* writeAt)
{
    spfsCacheInvalidateRequest* invalidator = new spfsCacheInvalidateRequest();
    FSHandle handle = writeAt->getFileDes()->metaData->handle;
    invalidator->setHandle(handle);
    invalidator->setOffset(writeAt->getOffset());
    invalidator->setDataType(writeAt->getDataType());
    invalidator->setCount(writeAt->getCount());

    return invalidator;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
