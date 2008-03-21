#ifndef IO_APPLICATION_H
#define IO_APPLICATION_H
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

#include <omnetpp.h>
#include "io_trace.h"
class FileDescriptor;
class spfsCacheInvalidateRequest;
class spfsMPIDirectoryCreateRequest;
class spfsMPIFileCloseRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileUpdateTimeRequest;
class spfsMPIFileWriteAtRequest;

/**
 * Model of an application process.
 */
class IOApplication : public cSimpleModule
{
public:
    /** Constructor */
    IOApplication();
    
    /** @return the file descriptor for a file id */
    FileDescriptor* getDescriptor(int fileId) const;

    /** @return the MPI application's canonical process rank */
    int getRank() const {return rank_;};

protected:
    /** Associate the fileId with a file descriptor */
    void setDescriptor(int fileId, FileDescriptor* descriptor);

    /**
     * Disassociate the fileId with its file descriptor
     *
     * @return the disassociated file descriptor
     */
    FileDescriptor* removeDescriptor(int FileId);
    
    /** Send out the required cache invalidation messages */
    void invalidateCaches(spfsMPIFileWriteAtRequest* writeAt);
    
    /** Create a cache invalidation message for sending to peers */
    spfsCacheInvalidateRequest* createCacheInvalidationMessage(
                                spfsMPIFileWriteAtRequest* writeAt);

    /** Create the file system files for this trace */
    virtual void populateFileSystem() = 0;

    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    virtual void handleSelfMessage(cMessage* msg);
    virtual void handleIOMessage(cMessage* msg);
    virtual void handleMPIMessage(cMessage* msg);

    /** FIXME: this should not be needed -- a bad hack */
    virtual cMessage* createMessage(void *) = 0;                

    /** process rank */
    int rank_;

    /** gate ids */
    int ioInGate_;
    int ioOutGate_;
    int mpiOutGate_;
    int mpiInGate_;
    bool msgScheduled_;

private:
    /** @return true if the next message was able to be scheduled */
    virtual bool scheduleNextMessage() = 0;

    /** Map of file descriptors keyed by descriptor ID */
    std::map<int, FileDescriptor*> descriptorById_;

    /** Fixed data collection */
    double totalCpuPhaseTime_;
    double applicationCompletionTime_;
    
    /** Temporal timing data collections */
    cOutVector cpuPhaseDelay_;
    cOutVector directoryCreateDelay_;
    cOutVector fileOpenDelay_;
    cOutVector fileReadDelay_;
    cOutVector fileWriteDelay_;
    cOutVector fileUpdateTimeDelay_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
