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
#include "comm_man.h"
#include "io_application.h"
#include "phtf_io_trace.h"
class FileDescriptor;
class Filename;
class spfsCacheInvalidateRequest;
class spfsMPIBarrierRequest;
class spfsMPIBcastRequest;
class spfsMPIDirectoryCreateRequest;
class spfsMPIFileCloseRequest;
class spfsMPIFileDeleteRequest;
class spfsMPIFileOpenRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileReadRequest;
class spfsMPIFileStatRequest;
class spfsMPIFileUpdateTimeRequest;
class spfsMPIFileWriteAtRequest;
class spfsMPIFileWriteRequest;
class spfsMPIRequest;
class DataType;

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

    /** Override ioApplication message handler */
    virtual void handleIOMessage(cMessage* msg);
    virtual void handleMPIMessage(cMessage* msg);

    /** Override ioApplication scheduleNextMessage() */
    virtual bool scheduleNextMessage();

    /** Create a cMessage from a trace record */
    spfsMPIRequest* createRequest(PHTFEventRecord* rec);

private:
    /** Join the correct communicators on rank change */
    virtual void rankChanged(int oldRank);

    /** Create the file system files for this trace */
    void populateFileSystem();

    /**
     * Perform event record processing
     * @return whether a message was scheduled
     */
    bool processEvent(PHTFEventRecord& event);

    /**
     * Perform event record processing for events that involve
     * weird processing
     *
     * @return whether a message was scheduled
     */
    bool processIrregularEvent(PHTFEventRecord* event);

    /** @return the request for the event record */
    cMessage* createMessage(const PHTFEventRecord* record);

    /** Schedule a self message as a trigger after CPU_PHASE */
    void scheduleCPUMessage(cMessage *msg);

    /** Dealing with barrier message */
    void handleBarrier(cMessage *msg, bool active = false);

    /** Add an open entry into the cache directly */
    void performFakeOpenProcessing(const PHTFEventRecord& openRecord);

    /** Perform the application processing to do an open */
    void performOpenProcessing(PHTFEventRecord* openRecord,
                               uint64_t& outCommunicatorId);

    /** Perform the application processing to simulate a seek */
    void performSeekProcessing(PHTFEventRecord* seekRecord);

    /** Perform the application processing to simulate a wait */
    void performWaitProcessing(PHTFEventRecord* waitRecord,
                               bool& outWaitIsComplete);

    void performSetViewProcessing(PHTFEventRecord* setViewRecord);

    /** Perform the application processing to create a new communicator */
    void performCommProcessing(PHTFEventRecord* commRecord);

    void performCreateCommunicator(std::string newcomm);

    void performDuplicateCommunicator(const Communicator& oldComm,
                                      Communicator& newComm);

    /** Perform the application processing to do create a new datatype */
    void performTypeProcessing(PHTFEventRecord* typeRecord);

    /** Recursively creates datatypes by datatype-id */
    void performCreateDataType(std::string typeId);
    void performCreateBasicDataType(std::string typeId);
    void performCreateContiguousDataType(std::string typeId);
    void performCreateStructDataType(std::string typeId);
    void performCreateSubarrayDataType(std::string typeId);
    void performCreateVectorDataType(std::string typeId);

    /** @return a ALL_REDUCE request */
    spfsMPIBarrierRequest* createAllReduceMessage(
        const PHTFEventRecord* allreduce);

    /** @return a BARRIER request */
    spfsMPIBarrierRequest* createBarrierMessage(
        const PHTFEventRecord* barrier);

    /** @return a BCAST request */
    spfsMPIBcastRequest* createBcastMessage(
        const PHTFEventRecord* bcast);

    /** @return an CPU Phase Message */
    cMessage* createCPUPhaseMessage(
        const PHTFEventRecord* cpuRecord);

    /** @return an MPIO_Wait Message */
    cMessage* createWaitMessage(
        const PHTFEventRecord* waitRecord);

    /** @return an MPI File Close request */
    spfsMPIFileCloseRequest* createFileCloseMessage(
        const PHTFEventRecord* closeRecord);

    /** @return an MPI File Delete request */
    spfsMPIFileDeleteRequest* createFileDeleteMessage(
        const PHTFEventRecord* deleteRecord);

    /** @return an MPI_FILE_GET_INFO request */
    spfsMPIFileStatRequest* createFileGetInfoMessage(
        const PHTFEventRecord* getInfoRecord);

    /** @return an MPI_FILE_GET_SIZE */
    spfsMPIFileStatRequest* createFileGetSizeMessage(
        const PHTFEventRecord* getSizeRecord);

    /** @return an MPI File IRead request */
    spfsMPIFileReadAtRequest * createFileIReadMessage(
        const PHTFEventRecord* readRecord);

    /** @return an MPI File IWrite request */
    spfsMPIFileWriteAtRequest * createFileIWriteMessage(
        const PHTFEventRecord* writeRecord);

    /** @return an MPI File Open request */
    spfsMPIFileOpenRequest* createFileOpenMessage(
        const PHTFEventRecord* openRecord);

    /** @return an MPI File Read At request */
    spfsMPIFileReadAtRequest* createFileReadAtMessage(
        const PHTFEventRecord* readAtRecord);

    /** @return an MPI File Read request */
    spfsMPIFileReadAtRequest * createFileReadMessage(
        const PHTFEventRecord* readRecord);

    /** @return an MPI File Set Size Message */
    spfsMPIFileStatRequest* createFileSetSizeMessage(
        const PHTFEventRecord* setSizeRecord);

    /** @return an MPI File Update Time request */
    spfsMPIFileUpdateTimeRequest* createFileUpdateTimeMessage(
        const PHTFEventRecord* utimeRecord);

    /** @return an MPI File Write At request */
    spfsMPIFileWriteAtRequest* createFileWriteAtMessage(
        const PHTFEventRecord* writeAtRecord);

    /** @return an MPI File Write request */
    spfsMPIFileWriteAtRequest * createFileWriteMessage(
        const PHTFEventRecord* writeRecord);

    /** @return a BCAST for the communicator */
    spfsMPIBcastRequest* createBcastRequest(int communicatorId);

    /** Trace file location */
    std::string traceDirectory_;

    /** PHTF Event File */
    PHTFEvent* phtfEvent_;

    /** Map of non-blocking IO request that are still pending */
    std::map<long, cMessage*> pendingRequestsById_;

    /** Map of defined Datatypes */
    std::map<std::string, DataType*> dataTypeById_;

    /** retrieve Datatype from map, NULL if none found */
    DataType * getDataTypeById(std::string);

    /** This is a reckless approach to passing around the record ID */
    long rec_id_;

    /** Flag to indicate if CPU pauses should be ignored */
    bool disableCPUPause_;
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
