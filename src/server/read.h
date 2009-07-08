#ifndef READ_H
#define READ_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsReadRequest;
class FSServer;

/**
 * Perform server side read processing
 */
class Read
{
public:

    /** Constructor */
    Read(FSServer* module, spfsReadRequest* readReq);

    /** Destructor */
    ~Read();

    /**
     * Perform server side read processing
     */
    void handleServerMessage(cMessage* msg);

protected:

    /** @return true if the file contains the offset/extent to be read */
    bool hasReadData();

    /**
     * Create and send the data flow requests
     */
    void startDataFlow();

    /**
     * Create and send the final read response
     */
    void sendFinalResponse();

    /**
     * No-op for now.
     */
    void finish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating read request */
    spfsReadRequest* readReq_;

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
