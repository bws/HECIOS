#ifndef FS_READ_OPERATION_H_
#define FS_READ_OPERATION_H_
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
class spfsMPIFileReadAtRequest;

/** Class responsible for performing client-side file reads */
class FSReadOperation : public FSClientOperation
{
public:
    /** Construct operation */
    FSReadOperation(FSClient* client, spfsMPIFileReadAtRequest* readRequest);
protected:
    /** Register state machines to perform the read */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** The file system client module */
    FSClient* client_;

    /** The originating MPI Read request */
    spfsMPIFileReadAtRequest* readAtRequest_;
};

#endif /* FS_READ_OPERATION_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
