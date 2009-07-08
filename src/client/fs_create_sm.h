#ifndef FS_CREATE_SM_H
#define FS_CREATE_SM_H
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
 * Class responsible for opening a file
 */
class FSCreateSM : public FSStateMachine
{
public:
    /** Construct the file create state machine */
    FSCreateSM(const Filename& filename,
               spfsMPIRequest* mpiReq,
               FSClient* client);

protected:
    /** Message processing for serial creates */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send the metadata creation message */
    void createMeta();

    /** Send the metadata creation message */
    void createDataObjects();

    /** Send the metadata creation message */
    void countDataCreationResponse();

    /** @return true if all the data objects have been create */
    bool isDataCreationComplete();

    /** Send the message to set the metadata */
    void writeAttributes();

    /** Send the directory entry creation message */
    void createDirEnt();

    /** Add the file's new attributes to the client cache */
    void updateClientCache();

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
