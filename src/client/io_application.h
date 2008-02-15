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
class IOTrace;
class SHTFIOTrace;
class UMDIOTrace;
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
    IOApplication() : cSimpleModule(), trace_(0), rank_(-1) {};
    
    /** @return the file descriptor for a file id */
    FileDescriptor* getDescriptor(int fileId) const;

    /** @return the MPI application's canonical process rank */
    int getRank() const {return rank_;};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Get the next message to send */
    virtual cMessage* getNextMessage();

    /** Create a cMessage from an IOTrace::Record */
    virtual cMessage* createMessage(IOTrace::Record* rec);
    
    /** Assiciate the fileId with a file descriptor */
    void setDescriptor(int fileId, FileDescriptor* descriptor);

    /** Send out the required cache invalidation messages */
    void invalidateCaches(spfsMPIFileWriteAtRequest* writeAt);
    
    /** Create a cache invalidation message for sending to peers */
    spfsCacheInvalidateRequest* createCacheInvalidationMessage(
        spfsMPIFileWriteAtRequest* writeAt);
    
private:
    /** Create the file system files for this trace */
    void populateFileSystem();
    
    /** @return create the IOTrace for traceFilename */
    IOTrace* createIOTrace(const std::string& traceFilename);

    /** @return a UMDIOTrace for traceFilename */
    UMDIOTrace* createUMDIOTrace(std::string traceFilename);

    /** @return a SHTFIOTrace for traceFilename */
    SHTFIOTrace* createSHTFIOTrace(const std::string& traceFilename);

    /** @return an MPI DirectoryCreate request */
    spfsMPIDirectoryCreateRequest* createDirectoryCreateMessage(
        const IOTrace::Record* mkdirRecord);
    
    /** @return an MPI File Close request */
    spfsMPIFileCloseRequest* createCloseMessage(
        const IOTrace::Record* closeRecord);
    
    /** @return an MPI File Open request */
    spfsMPIFileOpenRequest* createOpenMessage(
        const IOTrace::Record* openRecord);
    
    /** @return an MPI File Read At request */
    spfsMPIFileReadAtRequest* createReadAtMessage(
        const IOTrace::Record* readAtRecord);
    
    /** @return an MPI File Read At request */
    spfsMPIFileReadAtRequest* createReadMessage(
        const IOTrace::Record* readRecord);
    
    /** @return an MPI File Update Time request */
    spfsMPIFileUpdateTimeRequest* createUpdateTimeMessage(
        const IOTrace::Record* utimeRecord);
    
    /** @return an MPI File Write At request */
    spfsMPIFileWriteAtRequest* createWriteAtMessage(
        const IOTrace::Record* writeAtRecord);
    
    /** @return an MPI File Write At request */
    spfsMPIFileWriteAtRequest* createWriteMessage(
        const IOTrace::Record* writeRecord);    
    
    IOTrace* trace_;
    int rank_;
    int ioInGate_;
    int ioOutGate_;
    int mpiOutGate_;
    int mpiInGate_;

    /** */
    std::map<int, FileDescriptor*> descriptorById_;

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
