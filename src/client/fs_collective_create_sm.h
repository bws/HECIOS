#ifndef FS_COLLECTIVE_CREATE_SM_H
#define FS_COLLECTIVE_CREATE_SM_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "filename.h"
#include "fs_state_machine.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIRequest;

/**
 * Class performing client side file collective create processing
 */
class FSCollectiveCreateSM : public FSStateMachine
{
public:
    /** Construct collective create state machine */
    FSCollectiveCreateSM(const Filename& filename,
                         spfsMPIRequest* mpiReq,
                         FSClient* client);

protected:
    /** Message processing for collective creates */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send the collective creation message */
    void collectiveCreate();

    /** The name of the file to create */
    Filename createFilename_;

    /** The originating MPI request */
    spfsMPIRequest* mpiReq_;

    /** The filesystem client module */
    FSClient* client_;
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
