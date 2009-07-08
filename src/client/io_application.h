#ifndef IO_APPLICATION_H
#define IO_APPLICATION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
#include <string>
#include "direct_message_interface.h"
#include "io_trace.h"
class FileDescriptor;

/**
 * Model of an application process.
 */
class IOApplication : public cSimpleModule, public DirectMessageInterface
{
public:
    /** Name string for CPU Phase Messages */
    static const char* CPU_PHASE_MESSAGE_NAME;

    /** Constructor */
    IOApplication();

    /** @return the file descriptor for a file id */
    FileDescriptor* getDescriptor(int fileId) const;

    /** Set the process rank for this application */
    void setRank(int rank);

    /** @return the MPI application's canonical process rank */
    int getRank() const { return rank_; };

    /** @return the application process' total file read data size */
    double getNumReadBytes() const { return totalBytesRead_; };

    /** @return the application process' total file write data size */
    double getNumWriteBytes() const { return totalBytesWritten_; };

    /** @return the application process' total file read time */
    double getReadTime() const { return totalReadTime_; };

    /** @return the application process' total file write time */
    double getWriteTime() const { return totalWriteTime_; };

    /** Interface to send a message directly to this IOApplication */
    void directMessage(cMessage* msg);

protected:
    /** Associate the fileId with a file descriptor */
    void setDescriptor(int fileId, FileDescriptor* descriptor);

    /**
     * Disassociate the fileId with its file descriptor
     *
     * @return the disassociated file descriptor
     */
    FileDescriptor* removeDescriptor(int FileId);

    /** Create the file system files for this trace */
    virtual void populateFileSystem() = 0;

    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Perform processing for self messages */
    virtual void handleSelfMessage(cMessage* msg);

    /** Perform processing for messages from the IO gates */
    virtual void handleIOMessage(cMessage* msg);

    /** Perform processing for messages from the MPI gates */
    virtual void handleMPIMessage(cMessage* msg);

    /** gate ids */
    int ioInGate_;
    int ioOutGate_;
    int mpiOutGate_;
    int mpiInGate_;

private:
    /** Invoked when the rank has been changed */
    virtual void rankChanged(int oldRank) = 0;

    /** @return true if the next message was able to be scheduled */
    virtual bool scheduleNextMessage() = 0;

    /** process rank */
    int rank_;

    /** Map of file descriptors keyed by descriptor ID */
    std::map<int, FileDescriptor*> descriptorById_;

    /** Fixed data collection */
    double totalCpuPhaseTime_;
    double applicationCompletionTime_;
    double totalBytesRead_;
    double totalBytesWritten_;
    double totalReadTime_;
    double totalWriteTime_;
    double totalReadBandwidth_;
    double totalWriteBandwidth_;

    /** Temporal timing data collections */
    cOutVector cpuPhaseDelay_;
    cOutVector directoryCreateDelay_;
    cOutVector directoryReadDelay_;
    cOutVector directoryRemoveDelay_;
    cOutVector fileCloseDelay_;
    cOutVector fileDeleteDelay_;
    cOutVector fileOpenDelay_;
    cOutVector fileReadDelay_;
    cOutVector fileStatDelay_;
    cOutVector fileUpdateTimeDelay_;
    cOutVector fileWriteDelay_;
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
