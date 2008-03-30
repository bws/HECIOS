#ifndef PHTF_IO_APPLICATION_H
#define PHTF_IO_APPLICATION_H
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
#include <map>
#include "io_application.h"
#include "phtf_io_trace.h"

class FileDescriptor;
class PHTFTrace;
class PHTFEvent;
class PHTFEventRecord;
class spfsCacheInvalidateRequest;
class spfsMPIDirectoryCreateRequest;
class spfsMPIFileCloseRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadRequest;
class spfsMPIFileUpdateTimeRequest;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteRequest;

/**
 * Model of an application process.
 */
class PHTFIOApplication : public IOApplication
{
public:
    /** Constructor */
    PHTFIOApplication() :IOApplication()  {waitReqId_ = -1;};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    virtual bool scheduleNextMessage();

    /** Create a cMessage from an IOTrace::Record */
    virtual cMessage* createMessage(PHTFEventRecord* rec);

    virtual void handleIOMessage(cMessage* msg);
    virtual void handleMPIMessage(cMessage* msg);

private:
    /** Create the file system files for this trace */
    void populateFileSystem();

    void scheduleCPUMessage(cMessage *msg);

    void handleBarrier(cMessage *msg, bool active = false);

    cMessage* createBarrierMessage(
        const PHTFEventRecord* barrierRecord);

    cMessage* createCPUPhaseMessage(
        const PHTFEventRecord* cpuRecord);
    cMessage* createWaitMessage(
        const PHTFEventRecord* waitRecord);
       
    /** @return an MPI File Close request */
    spfsMPIFileCloseRequest* createCloseMessage(
        const PHTFEventRecord* closeRecord);
    
    /** @return an MPI File Open request */
    spfsMPIFileOpenRequest* createOpenMessage(
        const PHTFEventRecord* openRecord);
    
    /** @return an MPI File Read At request */
    spfsMPIFileReadAtRequest* createReadAtMessage(
        const PHTFEventRecord* readAtRecord);
    
    /** @return an MPI File Update Time request */
    spfsMPIFileUpdateTimeRequest* createUpdateTimeMessage(
        const PHTFEventRecord* utimeRecord);
    
    /** @return an MPI File Write At request */
    spfsMPIFileWriteAtRequest* createWriteAtMessage(
        const PHTFEventRecord* writeAtRecord);

    spfsMPIFileReadAtRequest * createReadMessage(
        const PHTFEventRecord* readRecord);

    spfsMPIFileWriteAtRequest * createWriteMessage(
        const PHTFEventRecord* writeRecord);

    spfsMPIFileReadAtRequest * createIReadMessage(
        const PHTFEventRecord* readRecord);

    spfsMPIFileWriteAtRequest * createIWriteMessage(
        const PHTFEventRecord* writeRecord);

    PHTFEvent * phtfEvent_;
    PHTFEventRecord phtfRecord_;

    std::map<long, cMessage*> nonBlockingReq_;
    long waitReqId_;

    cMessage * context_;
    long desc_;
    int group_;

    bool noGetNext_;
    bool blocked_;
    
    int counter_;
    int sum_;
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
