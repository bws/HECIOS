#ifndef COLLECTIVE_GET_ATTR_H
#define COLLECTIVE_GET_ATTR_H
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
class spfsCollectiveGetAttrRequest;
class FSServer;

/**
 * State machine for performing collective get attribute processing
 */
class CollectiveGetAttr
{
public:
    /** Constructor */
    CollectiveGetAttr(FSServer* module, spfsCollectiveGetAttrRequest* getAttrReq);

    /**
     * Handle message as part of the get attributes process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the get attributes message to the OS
     */
    void getLocalAttributes();

    /** Send the child collective create requests for this node */
    void sendCollectiveRequests();

    /**
     *Process responses from servers and OS
     *
     * @return true if no more outstanding responses exist
     */
    bool processResponse(cMessage* response);

    /**
     * Send the final response to the client
     */
    void enterFinish();

private:
    /** Create a child collective request */
    spfsCollectiveGetAttrRequest* createChildCollectiveRequest(
        int idx, int numHandles) const;

    /** The parent module */
    FSServer* module_;

    /** The originating collective create request */
    spfsCollectiveGetAttrRequest* getAttrReq_;
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
