#ifndef WRITE_H
#define WRITE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsWriteRequest;
class FSServer;

/**
 * State machine performing the server write process
 */
class Write
{
public:

    /** Constructor */
    Write(FSServer* module, spfsWriteRequest* writeReq);

    /**
     * Handle server message
     */
    void handleServerMessage(cMessage* msg);

protected:

    /** Start the server side data flow processing for this write */
    void startDataFlow();

    /** Send the response indicating flow processing is ready */
    void sendResponse();

    /** Send response indicating the data has been commited to storage */
    void sendCompletionResponse();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating write request */
    spfsWriteRequest* writeReq_;
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
