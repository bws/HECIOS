#ifndef READ_PAGES_H
#define READ_PAGES_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <set>
#include <vector>
#include "file_page.h"
class cMessage;
class spfsInvalidatePagesRequest;
class spfsReadPagesRequest;
class FSServer;

/**
 * Perform server side read processing
 */
class ReadPages
{
public:

    /** Constructor */
    ReadPages(FSServer* module, spfsReadPagesRequest* readReq);

    /** Destructor */
    ~ReadPages();

    /**
     * Perform server side read processing
     */
    void handleServerMessage(cMessage* msg);

protected:

    /** @return true if the file contains the offset/extent to be read */
    bool hasReadData();

    /** Partition the request pages into the client-exclusive and server-local sets */
    void parititionRequestPages(std::set<FilePageId>& outClientPages,
                                std::set<FilePageId>& outServerPages);

    /** @return the set of page forward requests */
    std::vector<spfsInvalidatePagesRequest*> createInvalidatePagesRequests(
        const std::set<FilePageId>& clientPages);

    /** Create and send the data flow requests */
    void startDataFlow(const std::set<FilePageId>& localPages);

    /**
     * Create and send the cache forwarding requests
     */
    void sendInvalidateRequests(std::vector<spfsInvalidatePagesRequest*> invalidations);

    /**
     * Create and send the final read response
     */
    void sendFinalResponse(const std::set<FilePageId>& clientPages,
                           const std::set<FilePageId>& serverPages);

    /**
     * No-op for now.
     */
    void finish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating read request */
    spfsReadPagesRequest* readReq_;

    /** Flag indicating the orginating request should be deleted on exit */
    bool cleanupRequest_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
