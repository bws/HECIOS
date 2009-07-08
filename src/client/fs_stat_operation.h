#ifndef FS_STAT_OPERATION_H
#define FS_STAT_OPERATION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_client_operation.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIFileStatRequest;

/**
 * Class responsible for performing a file stat
 */
class FSStatOperation : public FSClientOperation
{
public:
    /** Construct FS Stat processor for a file */
    FSStatOperation(FSClient* client,
                    spfsMPIFileStatRequest* deleteReq,
                    bool useCollectiveCommunication);

protected:
    /** Register the state machines to perform a file stat */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI stat request */
    spfsMPIFileStatRequest* statReq_;

    /** Use server to server based collectives to delete file */
    bool useCollectiveCommunication_;
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
