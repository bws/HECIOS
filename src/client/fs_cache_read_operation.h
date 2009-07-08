#ifndef FS_CACHE_READ_OPERATION_H_
#define FS_CACHE_READ_OPERATION_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <set>
#include "fs_operation.h"
class cFSM;
class cMessage;
class FSClient;
class spfsCacheReadRequest;
class spfsCacheReadResponse;
class spfsCacheReadExclusiveRequest;
class spfsCacheReadSharedRequest;

/** Class responsible for performing client-side file reads */
class FSCacheReadOperation : public FSOperation
{
public:
    /** Construct operation for read exclusive */
    FSCacheReadOperation(FSClient* client, spfsCacheReadExclusiveRequest* readRequest);

    /** Construct operation for read shared */
    FSCacheReadOperation(FSClient* client, spfsCacheReadSharedRequest* readRequest);

    /** Destructor */
    ~FSCacheReadOperation();

protected:
    /** Register state machines to perform the read */
    virtual void registerStateMachines();

    /** Send final response */
    virtual void sendFinalResponse();

private:

    /** Set the server page ids field in the cache read response */
    void setServerPageIds(spfsCacheReadResponse* readResponse);

    /** The file system client module */
    FSClient* client_;

    /** The originating cache read request */
    spfsCacheReadRequest* readRequest_;

    /** Indicate whether this is a read exclusive or read shared */
    bool isExclusive_;
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
