#ifndef FS_READ_SM_H
#define FS_READ_SM_H
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
class FSClient;
class spfsMPIFileReadAtRequest;
class spfsDataFlowFinish;
class spfsReadRequest;

/**
 * Class responsible for removing a file
 */
class FSReadSM : public FSStateMachine
{
public:
    /** Construct the file read state machine */
    FSReadSM(spfsMPIFileReadAtRequest* mpiReq, FSClient* client);

protected:
    /** Message processing for removes */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /**  Construct server read requests */
    virtual void enterRead();

    /**
     * @return true if the read data exists in the file
     * TODO: The implementation of this function is currently naive/wrong
     */
    bool fileHasReadData(std::size_t reqBytes);

    /** Count a finished flow */
    void countFlowFinish(spfsDataFlowFinish* finishMsg);

    /** Count a read response */
    void countResponse();

    /** @return true if all read responses and finished flows are received */
    bool isReadComplete();

    /** Start a flow */
    void startFlow(spfsReadRequest* serverRequest);

    /** The originating MPI request */
    spfsMPIFileReadAtRequest* readRequest_;

    /** The file system client module */
    FSClient* client_;

    /** The number of bytes read */
    std::size_t bytesRead_;
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
