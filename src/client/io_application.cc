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
#include "phtf_io_trace.h"
#include "comm_man.h"

using namespace std;

// OMNet Registriation Method

IOApplication::IOApplication()
    : cSimpleModule(),
      rank_(-1),
      cpuPhaseDelay_("SPFS CPU Phase Delay"),
      directoryCreateDelay_("SPFS MPI Directory Create Delay"),
      fileOpenDelay_("SPFS MPI File Open Delay"),
      fileReadDelay_("SPFS MPI File Read Delay"),
      fileWriteDelay_("SPFS MPI File Write Delay"),
      fileUpdateTimeDelay_("SPFS MPI File Update Time Delay")
{
}

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

    msgScheduled_ = false;

    // Set the process rank
    rank_ = CommMan::getInstance()->joinComm(MPI_COMM_WORLD, 0);

    // Initialize scalar data collection values
    totalCpuPhaseTime_ = 0.0;
    applicationCompletionTime_ = 0.0;
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

    // Reset the rank generator to 0

    recordScalar("SPFS Total CPU Phase Delay", totalCpuPhaseTime_);
    recordScalar("SPFS App. Completion Time", applicationCompletionTime_);

}

void IOApplication::handleSelfMessage(cMessage* msg)
{
    // Create file system files only once
    static bool fileSystemPopulated = false;
    if (!fileSystemPopulated)
    {
        populateFileSystem();
        fileSystemPopulated = true;
    }

    // Record the length of this CPU Phase
    if (0 == strcmp("CPU Phase", msg->name()))
    {
        // Determine the request response roundtrip time
        simtime_t phaseBeginTime = msg->creationTime();
        simtime_t phaseEndTime = simTime();
        simtime_t delay = phaseBeginTime - phaseEndTime;
        cpuPhaseDelay_.record(delay);
        totalCpuPhaseTime_ += delay;
    }
    
    // Schedule the next message
    msgScheduled_ = scheduleNextMessage();
    delete msg;
}

void IOApplication::handleIOMessage(cMessage* msg)
{
    // Determine the request response roundtrip time
    cMessage* parentRequest = static_cast<cMessage*>(msg->contextPointer());
    simtime_t reqSendTime = parentRequest->creationTime();
    simtime_t respArriveTime = simTime();
    simtime_t delay = respArriveTime - reqSendTime;

    // Collect statistics for the operation's total delay
    switch(msg->kind())
    {
        case SPFS_MPI_DIRECTORY_CREATE_RESPONSE:
        {
            directoryCreateDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_CLOSE_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_DELETE_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_GET_INFO_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_OPEN_RESPONSE:
        {
            fileOpenDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_SET_INFO_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_SET_SIZE_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_IREAD_RESPONSE:
        case SPFS_MPI_FILE_READ_AT_RESPONSE:
        case SPFS_MPI_FILE_READ_RESPONSE:
        {
            fileReadDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_UPDATE_TIME_RESPONSE:
        {
            fileUpdateTimeDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_IWRITE_RESPONSE:
        case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
        case SPFS_MPI_FILE_WRITE_RESPONSE:
        {
            fileWriteDelay_.record(delay);
            break;
        }
        default:
            cerr << "ERROR: " << __FILE__ << ":" << __LINE__
                 << ":handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
    }
    
    // Schedule the next message
    msgScheduled_ = scheduleNextMessage();
    
    // Delete the originating request
    delete parentRequest;
    
    // Delete the response
    delete msg;
}

void IOApplication::handleMPIMessage(cMessage* msg)
{
// override in subclass
}

/**
 * Handle MPI-IO Response messages
 */
void IOApplication::handleMessage(cMessage* msg)
{   
    if (msg->isSelfMessage())
    {
        handleSelfMessage(msg);
    }
    else if (msg->arrivalGateId() == ioInGate_)
    {
        handleIOMessage(msg);
    }
    else if (msg->arrivalGateId() == mpiInGate_)
    {
        handleMPIMessage(msg);
    }

    if (!msgScheduled_)
    {
        cerr << "Rank " << rank_ << " IOApplication Time: " << simTime()
             << ": No more messages to post." << endl;
        applicationCompletionTime_ = simTime();
    }
}

void IOApplication::setDescriptor(int fileId, FileDescriptor* descriptor)
{
    assert((0 == descriptor) && (-1 != fileId) ? false : true);
    assert((0 != descriptor) && (-1 == fileId) ? false : true);
    descriptorById_[fileId] = descriptor;
}

FileDescriptor* IOApplication::removeDescriptor(int fileId)
{
    FileDescriptor* descriptor = getDescriptor(fileId);
    assert(0 != descriptor);
    descriptorById_.erase(fileId);
    return descriptor;
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
    req->setCommunicator(MPI_COMM_WORLD);
    req->encapsulate(inval);
    send(req, mpiOutGate_);
}

spfsCacheInvalidateRequest* IOApplication::createCacheInvalidationMessage(
    spfsMPIFileWriteAtRequest* writeAt)
{
    spfsCacheInvalidateRequest* invalidator = new spfsCacheInvalidateRequest();
    /*
    FSHandle handle = writeAt->getFileDes()->metaData->handle;
    invalidator->setHandle(handle);
    invalidator->setOffset(writeAt->getOffset());
    invalidator->setDataType(writeAt->getDataType());
    invalidator->setCount(writeAt->getCount());
    */
    return invalidator;
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
