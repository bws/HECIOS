#ifndef FS_CLOSE_OPERATION_H
#define FS_CLOSE_OPERATION_H
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
class spfsMPIFileCloseRequest;

/**
 * Class responsible for closing a file
 */
class FSCloseOperation : public FSClientOperation
{
public:
    /** Construct FSClose processor */
    FSCloseOperation(FSClient* client, spfsMPIFileCloseRequest* closeReq);

protected:
    /** No op */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI Close request */
    spfsMPIFileCloseRequest* closeReq_;
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
