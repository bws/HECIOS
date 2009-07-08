#ifndef FS_WRITE_SM_H
#define FS_WRITE_SM_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include "fs_state_machine.h"
class cFSM;
class cMessage;
class FileDescriptor;
class FSClient;
class spfsMPIRequest;
class spfsDataFlowFinish;
class spfsMPIFileWriteAtRequest;
class spfsWriteCompletionResponse;
class spfsWriteResponse;

/**
 * Class responsible for writing a file
 */
class FSWriteSM : public FSStateMachine
{
public:
    /** Construct the file read state machine */
    FSWriteSM(spfsMPIFileWriteAtRequest* writeReq, FSClient* client);

protected:
    /** Message processing for removes */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send messages establishing the write flows */
    void beginWrite();

    /** Start a flow */
    void startFlow(spfsWriteResponse* writeResponse);

    /** Count write completions */
    void countCompletion(spfsWriteCompletionResponse* completionResponse);

    /** Count a finished flow */
    void countFlowFinish(spfsDataFlowFinish* finishMsg);

    /** Count a write response */
    void countResponse();

    /** @return true if all write flows and response messages are received */
    bool isWriteComplete();

    /** The originating MPI request */
    spfsMPIFileWriteAtRequest* writeRequest_;

    /** The filesystem client module */
    FSClient* client_;

    /** The number of bytes written */
    std::size_t bytesWritten_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
