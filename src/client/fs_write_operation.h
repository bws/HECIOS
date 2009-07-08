#ifndef FS_WRITE_OPERATION_H_
#define FS_WRITE_OPERATION_H_
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
class spfsMPIFileWriteAtRequest;

/** Class responsible for performing client-side file reads */
class FSWriteOperation : public FSClientOperation
{
public:
    /** Construct operation */
    FSWriteOperation(FSClient* client, spfsMPIFileWriteAtRequest* writeRequest);

protected:
    /** Register state machines to perform the read */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** The file system client module */
    FSClient* client_;

    /** The originating MPI Read request */
    spfsMPIFileWriteAtRequest* writeAtRequest_;
};



#endif /* FS_WRITE_OPERATION_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
