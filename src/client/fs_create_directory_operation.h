#ifndef FS_CREATE_DIRECTORY_OPERATION_H_
#define FS_CREATE_DIRECTORY_OPERATION_H_
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
class spfsMPIDirectoryCreateRequest;

class FSCreateDirectoryOperation : public FSClientOperation
{
public:
    /** Construct FS Delete processor for a file */
    FSCreateDirectoryOperation(FSClient* client,
                               spfsMPIDirectoryCreateRequest* dirCreateReq);

protected:
    /** Register the state machines to perform a file deletion */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI directory create request */
    spfsMPIDirectoryCreateRequest* createDirReq_;
};


#endif /* FS_CREATE_DIRECTORY_OPERATION_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
