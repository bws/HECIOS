#ifndef FS_READ_DIRECTORY_SM_H
#define FS_READ_DIRECTORY_SM_H
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

/**
 * Class responsible for removing a file
 */
class FSReadDirectorySM : public FSStateMachine
{
public:
    /** Construct the read directory state machine */
    FSReadDirectorySM(FileDescriptor* fd,
                      std::size_t numEntries,
                      spfsMPIRequest* mpiReq,
                      FSClient* client);

protected:
    /** Message processing for removes */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send the message to send the dir read request */
    void readDirEnt();

    /** The name of the file to remove */
    FileDescriptor* descriptor_;

    /** The number of directory entries to read */
    std::size_t numEntries_;

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
