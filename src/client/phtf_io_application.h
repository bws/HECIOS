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
class spfsCacheInvalidateRequest;
class spfsMPIBarrierRequest;
class spfsMPIBcastRequest;
class spfsMPIDirectoryCreateRequest;
class spfsMPIFileCloseRequest;
class spfsMPIFileDeleteRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadRequest;
class spfsMPIFileUpdateTimeRequest;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteRequest;
class spfsMPIRequest;

/**
 * Model of an application process.
 */
class PHTFIOApplication : public IOApplication
{
public:
    /** Constructor */
    PHTFIOApplication();
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Override handleMessage to handle Open processing */
    virtual void handleMessage(cMessage* msg);
    
    /** Override ioApplication scheduleNextMessage() */
    virtual bool scheduleNextMessage();

    /** Create a cMessage from a trace record */
    spfsMPIRequest* createRequest(PHTFEventRecord* rec);

    /** Override ioApplication message handler */
    virtual void handleIOMessage(cMessage* msg);
    virtual void handleMPIMessage(cMessage* msg);

private:
    /** Join the correct communicators on rank change */
    virtual void rankChanged(int oldRank);

    /** Create the file system files for this trace */
    void populateFileSystem();

    /** Schedule a self message as a trigger after CPU_PHASE */
    void scheduleCPUMessage(cMessage *msg);

    /** Dealing with barrier message */
    void handleBarrier(cMessage *msg, bool active = false);

    /** Perform the application processing to do an open */
    void performOpenProcessing(PHTFEventRecord* openRecord,
                               int& outCommunicatorId);
    
    /** Perform the application processing to simulate a seek */
    void performSeekProcessing(PHTFEventRecord* seekRecord);
    
    /** Perform the application processing to simulate a wait */
    void performWaitProcessing(PHTFEventRecord* waitRecord,
                               bool& outWaitIsComplete);
    
    /** @return a Barrier request */
    spfsMPIBarrierRequest* createBarrierMessage(
        const PHTFEventRecord* barrierRecord);

    /** @return a Bcast request */
    spfsMPIBcastRequest* createBcastRequest(int communicatorId);

    /** @return an CPU Phase Message */
    cMessage* createCPUPhaseMessage(
        const PHTFEventRecord* cpuRecord);

    /** @return an MPIO_Wait Message */
    cMessage* createWaitMessage(
        const PHTFEventRecord* waitRecord);

    /** @return an MPI File Seek Message */
    cMessage* createSeekMessage(
        const PHTFEventRecord* seekRecord);
       
    /** @return an MPI File Close request */
    spfsMPIFileCloseRequest* createCloseMessage(
        const PHTFEventRecord* closeRecord);
    
    /** @return an MPI File Delete request */
    spfsMPIFileDeleteRequest* createDeleteRequest(
        const PHTFEventRecord* deleteRecord);
    
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

    /** @return an MPI File Read request */
    spfsMPIFileReadAtRequest * createReadMessage(
        const PHTFEventRecord* readRecord);

    /** @return an MPI File Write request */
    spfsMPIFileWriteAtRequest * createWriteMessage(
        const PHTFEventRecord* writeRecord);

    /** @return an MPI File IRead request */
    spfsMPIFileReadAtRequest * createIReadMessage(
        const PHTFEventRecord* readRecord);

    /** @return an MPI File IWrite request */
    spfsMPIFileWriteAtRequest * createIWriteMessage(
        const PHTFEventRecord* writeRecord);
    
    /** PHTF Event File */
    PHTFEvent* phtfEvent_;

    /** Map of non-blocking IO request that are still pending */
    std::map<long, cMessage*> pendingRequestsById_;    
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
