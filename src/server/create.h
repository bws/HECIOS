#ifndef CREATE_H
#define CREATE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsCreateRequest;
class FSServer;

/**
 * State machine for performing data object creation processing
 */
class Create
{
public:
    /** Constructor */
    Create(FSServer* module, spfsCreateRequest* createReq);

    /**
     * Handle message as part of the creation process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the file creation message to the OS
     */
    void enterCreate();

    /**
     * Send the final response to the client
     */
    void enterFinish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating create request */
    spfsCreateRequest* createReq_;
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
