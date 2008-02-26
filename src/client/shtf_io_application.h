#ifndef SHTFIO_APPLICATION_H
#define SHTFIO_APPLICATION_H
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
#include "io_application.h"
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
class SHTFIOApplication : public IOApplication
{
public:
    /** Constructor */
    SHTFIOApplication():IOApplication(){};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    virtual bool scheduleNextMessage();

    /** Create a cMessage from an IOTrace::Record */
    virtual cMessage* createMessage(void *){return NULL;};
    virtual cMessage* createMessage(IOTrace::Record* rec);
    
private:
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
    
    /** @return an MPI File Close request */
    spfsMPIFileCloseRequest* createCloseMessage(
        const IOTrace::Record* closeRecord);
    
    /** @return an MPI File Open request */
    spfsMPIFileOpenRequest* createOpenMessage(
        const IOTrace::Record* openRecord);
    
    /** @return an MPI File Read At request */
    spfsMPIFileReadAtRequest* createReadAtMessage(
        const IOTrace::Record* readAtRecord);

    spfsMPIFileReadAtRequest* createReadMessage(
        const IOTrace::Record* readRecord);
    
    /** @return an MPI File Update Time request */
    spfsMPIFileUpdateTimeRequest* createUpdateTimeMessage(
        const IOTrace::Record* utimeRecord);
    
    /** @return an MPI File Write At request */
    spfsMPIFileWriteAtRequest* createWriteAtMessage(
        const IOTrace::Record* writeAtRecord);
    
    spfsMPIFileWriteAtRequest* createWriteMessage(
        const IOTrace::Record* writeRecord);
    
    IOTrace* trace_;
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
