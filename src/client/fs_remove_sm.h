#ifndef FS_REMOVE_SM_H
#define FS_REMOVE_SM_H
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
 * Class responsible for removing a file
 */
class FSRemoveSM : public FSStateMachine
{
public:
    /** Construct the file remove state machine */
    FSRemoveSM(const Filename& filename,
               spfsMPIRequest* mpiReq,
               FSClient* client);

protected:
    /** Message processing for removes */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send the directory entry remove message */
    void removeDirEnt();

    /** Send the metadata remove message */
    void removeMeta();

    /** Send the data object remove messages */
    void removeDataObjects();

    /** @return true if all the data objects have been create */
    bool countDataRemoveResponse();

    /** Remove entries from the client cache */
    void removeNameAndAttributesFromCache();

    /** The name of the file to remove */
    Filename removeName_;

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
