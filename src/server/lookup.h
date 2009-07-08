#ifndef LOOKUP_H
#define LOOKUP_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "pfs_types.h"
class cMessage;
class spfsLookupPathRequest;
class FSServer;

/**
 * Perform server name lookups
 */
class Lookup
{
public:
    /** Constructor */
    Lookup(FSServer* module, spfsLookupPathRequest* lookupReq);

    /** Handle server lookup request */
    void handleServerMessage(cMessage* msg);

protected:
    /** Local lookup status results */
    enum LookupStatus {INVALID_LOOKUP_STATUS = 0,
                       FULL_LOOKUP_COMPLETE,
                       LOCAL_LOOKUP_COMPLETE,
                       LOCAL_LOOKUP_INCOMPLETE,
                       LOCAL_LOOKUP_FAILED};

    /** Lookup the handle in the directory entries on disk */
    void lookupName();

    /** Determine the lookup result */
    LookupStatus processLookupResult();

    /** Send the final response */
    void finish(FSLookupStatus lookupStatus);

private:
    /** The parent module */
    FSServer* module_;

    /** The originating create request */
    spfsLookupPathRequest* lookupReq_;
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
