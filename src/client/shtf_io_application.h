#ifndef SHTFIO_APPLICATION_H
#define SHTFIO_APPLICATION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
#include "basic_data_type.h"
#include "io_application.h"
#include "io_trace.h"
class FileDescriptor;
class IOTrace;
class SHTFIOTrace;
class UMDIOTrace;
class spfsCacheInvalidateRequest;
class spfsMPIDirectoryCreateRequest;
class spfsMPIDirectoryReadRequest;
class spfsMPIDirectoryRemoveRequest;
class spfsMPIFileCloseRequest;
class spfsMPIFileDeleteRequest;
class spfsMPIFileGetAModeRequest;
class spfsMPIFileGetSizeRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileStatRequest;
class spfsMPIFileUpdateTimeRequest;
class spfsMPIFileWriteAtRequest;


/**
 * Model of an application process.
 */
class SHTFIOApplication : public IOApplication
{
public:
    /** Constructor */
    SHTFIOApplication();

protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    virtual bool scheduleNextMessage();

    /** Create a cMessage from an IOTrace::Record */
    virtual cMessage* createMessage(IOTrace::Record* rec);

private:
    /** No-op on rank changes */
    virtual void rankChanged(int oldRank) {};

    /** Create the file system files for this trace */
    virtual void populateFileSystem();

    /** @return create the IOTrace for traceFilename */
    IOTrace* createIOTrace(const std::string& traceFilename);

    /** @return a UMDIOTrace for traceFilename */
    UMDIOTrace* createUMDIOTrace(std::string traceFilename);

    /** @return a SHTFIOTrace for traceFilename */
    SHTFIOTrace* createSHTFIOTrace(const std::string& traceFilename);

    /** @return an MPI DirectoryCreate request */
    spfsMPIDirectoryCreateRequest* createDirectoryCreateMessage(
        const IOTrace::Record* mkdirRecord);

    /** @return an MPI DirectoryRead request */
    spfsMPIDirectoryReadRequest* createDirectoryReadMessage(
        const IOTrace::Record* readDirRecord);

    /** @return an MPI DirectoryRemove request */
    spfsMPIDirectoryRemoveRequest* createDirectoryRemoveMessage(
        const IOTrace::Record* rmDirRecord);

    /** @return an MPI File Close request */
    spfsMPIFileCloseRequest* createCloseMessage(
        const IOTrace::Record* closeRecord);

    /** @return an MPI File Delete request */
    spfsMPIFileDeleteRequest* createDeleteMessage(
        const IOTrace::Record* unlinkRecord);

    /** @return an MPI File Get AMode request */
    spfsMPIFileGetAModeRequest* createGetAModeMessage(
        const IOTrace::Record* accessRecord);

    /** @return an MPI File Get Size request */
    spfsMPIFileGetSizeRequest* createGetSizeMessage(
        const IOTrace::Record* statRecord);

    /** @return an MPI File Open request */
    spfsMPIFileOpenRequest* createOpenMessage(
        const IOTrace::Record* openRecord);

    /** @return an MPI File Read At request */
    spfsMPIFileReadAtRequest* createReadAtMessage(
        const IOTrace::Record* readAtRecord);

    /** @return an MPI File Read At request */
    spfsMPIFileReadAtRequest* createReadMessage(
        const IOTrace::Record* readRecord);

    /** @return an MPI File Stat request */
    spfsMPIFileStatRequest* createStatMessage(
        const IOTrace::Record* statRecord);

    /** @return an MPI File Update Time request */
    spfsMPIFileUpdateTimeRequest* createUpdateTimeMessage(
        const IOTrace::Record* utimeRecord);

    /** @return an MPI File Write At request */
    spfsMPIFileWriteAtRequest* createWriteAtMessage(
        const IOTrace::Record* writeAtRecord);

    /** @return an MPI File Write At request */
    spfsMPIFileWriteAtRequest* createWriteMessage(
        const IOTrace::Record* writeRecord);

    /** SHTF Tracefile for this application */
    IOTrace* trace_;

    /** Byte data type used by POSIX I/O applications */
    ByteDataType byteDataType_;
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
