#ifndef REMOVE_H
#define REMOVE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsRemoveRequest;
class FSServer;

/**
 * State machine for performing file remove processing
 */
class Remove
{
public:
    /** Constructor */
    Remove(FSServer* module, spfsRemoveRequest* removeReq);

    /**
     * Handle message as part of the creation process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the file unlink message to the OS
     */
    void unlinkFile();

    /**
     * Send the final response to the client
     */
    void finish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating remove request */
    spfsRemoveRequest* removeReq_;
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
