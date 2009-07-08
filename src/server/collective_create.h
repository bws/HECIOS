#ifndef COLLECTIVE_CREATE_H
#define COLLECTIVE_CREATE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "pfs_types.h"
class cArray;
class cMessage;
class spfsCollectiveCreateRequest;
class FSServer;

/**
 * State machine for performing collective data object creation processing
 */
class CollectiveCreate
{
public:
    /** Constructor */
    CollectiveCreate(FSServer* module, spfsCollectiveCreateRequest* createReq);

    /**
     * Handle message as part of the creation process
     */
    void handleServerMessage(cMessage* msg);

protected:
    /** Send the collective metadata creation message */
    void sendMeta();

    /** Send the file creation message to the OS */
    void enterCreate();

    /** Send a message to set the attributes */
    void setAttributes();

    /** Send the file write message to the OS */
    void createDirEnt();

    /** Send the child collective create requests for this node */
    void sendCollectiveRequests();

    /**
     * Process responses from servers and OS
     *
     * @return true if no more outstanding responses exist
     */
    bool processResponse(cMessage* response);

    /** Send the final response to the client */
    void enterFinish();

private:
    /** Create a child collective request */
    spfsCollectiveCreateRequest* createChildCollectiveRequest(
        int idx, int numHandles) const;

    /** The parent module */
    FSServer* module_;

    /** The originating collective create request */
    spfsCollectiveCreateRequest* createReq_;
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
