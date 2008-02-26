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

using namespace std;

// OMNet Registriation Method
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
            case SPFS_MPI_FILE_IREAD_RESPONSE:
            case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
            case SPFS_MPI_FILE_WRITE_RESPONSE:
            case SPFS_MPI_FILE_IWRITE_RESPONSE:
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
        cerr << "Rank " << rank_ << " IOApplication Time: " << simTime()
             << ": No more messages to post." << endl;
    }
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

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
