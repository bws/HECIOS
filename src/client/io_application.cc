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
#include "filename.h"
#include "file_builder.h"
#include "file_descriptor.h"
#include "io_trace.h"
#include "mpi_proto_m.h"
#include "storage_layout_manager.h"
#include "comm_man.h"

using namespace std;

// OMNet Registriation Method
const char* IOApplication::CPU_PHASE_MESSAGE_NAME = "CPU Phase";

IOApplication::IOApplication()
    : cSimpleModule(),
      rank_(-1),
      cpuPhaseDelay_("SPFS CPU Phase Delay"),
      directoryCreateDelay_("SPFS MPI Directory Create Delay"),
      directoryReadDelay_("SPFS MPI Directory Read Delay"),
      directoryRemoveDelay_("SPFS MPI Directory Remove Delay"),
      fileCloseDelay_("SPFS MPI File Close Delay"),
      fileDeleteDelay_("SPFS MPI File Delete Delay"),
      fileOpenDelay_("SPFS MPI File Open Delay"),
      fileReadDelay_("SPFS MPI File Read Delay"),
      fileStatDelay_("SPFS MPI File Stat Delay"),
      fileUpdateTimeDelay_("SPFS MPI File Update Time Delay"),
      fileWriteDelay_("SPFS MPI File Write Delay")
{
}

void IOApplication::setRank(int rank)
{
    int oldRank = rank_;
    rank_ = rank;
    rankChanged(oldRank);
}

void IOApplication::directMessage(cMessage* msg)
{
    Enter_Method("Application is receiving a direct message");
    take(msg);
    double delay = msg->par("Delay");
    scheduleAt(simTime() + delay, msg);
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

    // Initialize scalar data collection values
    totalCpuPhaseTime_ = 0.0;
    applicationCompletionTime_ = 0.0;
    totalBytesRead_ = 0.0;
    totalBytesWritten_ = 0.0;
    totalReadTime_ = 0.0;
    totalWriteTime_ = 0.0;
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

    // Record simulation statistics
    recordScalar("SPFS Total CPU Phase Delay", totalCpuPhaseTime_);
    recordScalar("SPFS App. Completion Time", applicationCompletionTime_);
    recordScalar("SPFS Total Bytes Read", totalBytesRead_);
    recordScalar("SPFS Total Bytes Written", totalBytesWritten_);
    recordScalar("SPFS Total Read Time (s)", totalReadTime_);
    recordScalar("SPFS Total Write Time (s)", totalWriteTime_);

    double readBandwidth = totalBytesRead_ / 1.0e6 / totalReadTime_;
    recordScalar("SPFS Total Read Bandwidth", readBandwidth);

    double writeBandwidth = totalBytesWritten_ / 1.0e6 / totalWriteTime_;
    recordScalar("SPFS Total Write Bandwidth", writeBandwidth);

    // Write some out to the terminal for easy verification
    cerr << "Process Completion Time: " << applicationCompletionTime_ << endl;
    cerr << "Process Total Write Bandwidth : " << writeBandwidth << " MB/s" << endl;
    cerr << "Process Total Read Bandwidth : " << readBandwidth << " MB/s" << endl;
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
    else
    {
        // Assume direct messages are I/O responses
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "ERROR: Unidentified message type received." << endl;
        assert(0);
    }

    bool messageScheduled = scheduleNextMessage();
    if (!messageScheduled)
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "No more messages to schedule." << endl;
        applicationCompletionTime_ = simTime();
    }
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
    if (0 == strcmp(CPU_PHASE_MESSAGE_NAME, msg->name()))
    {
        // Determine the request response roundtrip time
        simtime_t phaseBeginTime = msg->creationTime();
        simtime_t phaseEndTime = simTime();
        simtime_t delay = phaseEndTime - phaseBeginTime;
        cpuPhaseDelay_.record(delay);
        totalCpuPhaseTime_ += delay;

        // Cleanup the message
        delete msg;

    }
    else
    {
        // This is a file system response sent directly from the I/O middleware
        handleIOMessage(msg);
    }
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
        case SPFS_MPI_DIRECTORY_READ_RESPONSE:
        {
            directoryReadDelay_.record(delay);
            break;
        }
        case SPFS_MPI_DIRECTORY_REMOVE_RESPONSE:
        {
            directoryRemoveDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_CLOSE_RESPONSE:
        {
            fileCloseDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_DELETE_RESPONSE:
        {
            //cerr << "Delete Delay: " << delay << endl;
            fileDeleteDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_GET_INFO_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_OPEN_RESPONSE:
        {
            //cerr << "Open Delay: " << delay << endl;
            fileOpenDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
        {
            break;
        }
        case SPFS_MPI_FILE_READ_AT_RESPONSE:
        {
            fileReadDelay_.record(delay);

            // Collect aggregate statistics
            totalReadTime_ += delay;
            spfsMPIFileReadAtRequest* readAt =
                static_cast<spfsMPIFileReadAtRequest*>(parentRequest);
            DataType* dt = readAt->getDataType();
            totalBytesRead_ += (readAt->getCount() * dt->getExtent());
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
        case SPFS_MPI_FILE_STAT_RESPONSE:
        {
            //cerr << "Stat Delay: " << delay << endl;
            fileStatDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_UPDATE_TIME_RESPONSE:
        {
            fileUpdateTimeDelay_.record(delay);
            break;
        }
        case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
        {
            fileWriteDelay_.record(delay);

            // Collect aggregate statistics
            totalWriteTime_ += delay;
            spfsMPIFileWriteAtRequest* writeAt =
                static_cast<spfsMPIFileWriteAtRequest*>(parentRequest);
            DataType* dt = writeAt->getDataType();
            totalBytesWritten_ += (writeAt->getCount() * dt->getExtent());
            break;
        }
        default:
        {
            cerr << "ERROR: " << __FILE__ << ":" << __LINE__
                 << ":handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        }
    }

    // Delete the originating request
    delete parentRequest;

    // Delete the response
    delete msg;
}

void IOApplication::handleMPIMessage(cMessage* msg)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "This default implementation must fail" << endl;
    assert(false);
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

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
