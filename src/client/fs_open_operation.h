#ifndef FS_OPEN_OPERATION_H
#define FS_OPEN_OPERATION_H
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
class Filename;
class spfsMPIFileOpenRequest;

/**
 * Class responsible for opening a file
 */
class FSOpenOperation : public FSClientOperation
{
public:
    /** Construct FSOpen processor with collective optimization settable */
    FSOpenOperation(FSClient* client,
                    spfsMPIFileOpenRequest* openReq,
                    bool useCollectiveCommunication);

protected:
    /** Register state machines to perform open operation */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:
    /** @return true if this open creates a file */
    bool isFileCreate();

    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI Open request */
    spfsMPIFileOpenRequest* openReq_;

    /** Use server to server based collectives to create file */
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
