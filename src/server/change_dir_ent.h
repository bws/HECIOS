#ifndef CHANGE_DIR_ENT_H
#define CHANGE_DIR_ENT_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsChangeDirEntRequest;
class FSServer;

/**
 * State machine for performing directory entry change processing
 */
class ChangeDirEnt
{
public:
    /** Constructor */
    ChangeDirEnt(FSServer* module, spfsChangeDirEntRequest* changeDirEntReq);

    /**
     * Handle message as part of the change entry process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the file write message to the OS
     */
    void writeDirEnt();

    /**
     * Send the final response to the client
     */
    void finish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating change directory entry request */
    spfsChangeDirEntRequest* changeDirEntReq_;
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
