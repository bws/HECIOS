#ifndef CREATE_DIR_ENT_H
#define CREATE_DIR_ENT_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsCreateDirEntRequest;
class FSServer;

/**
 * State machine for performing directory entry creation processing
 */
class CreateDirEnt
{
public:
    /** Constructor */
    CreateDirEnt(FSServer* module, spfsCreateDirEntRequest* createDirEntReq);

    /**
     * Handle message as part of the creation process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the file creation message to the OS
     */
    void writeDirEnt();

    /**
     * Send the final response to the client
     */
    void finish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating create directory entry request */
    spfsCreateDirEntRequest* createDirEntReq_;
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
