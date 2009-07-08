#ifndef FS_DELETE_OPERATION_H
#define FS_DELETE_OPERATION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "filename.h"
#include "fs_client_operation.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIFileDeleteRequest;
class spfsMPIDirectoryRemoveRequest;

/**
 * Class responsible for deleting a file
 */
class FSDeleteOperation : public FSClientOperation
{
public:
    /** Construct FS Delete processor for a file */
    FSDeleteOperation(FSClient* client,
                      spfsMPIFileDeleteRequest* deleteReq,
                      bool useCollectiveCommunication);

    /** Construct FS Delete processor for a file */
    FSDeleteOperation(FSClient* client,
                      spfsMPIDirectoryRemoveRequest* rmDirReq,
                      bool useCollectiveCommunication);

protected:
    /** Register the state machines to perform a file deletion */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI delete/remove request */
    spfsMPIRequest* deleteReq_;

    /** Use server to server based collectives to delete file */
    bool useCollectiveCommunication_;

    /** The name to delete */
    Filename deleteName_;
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
